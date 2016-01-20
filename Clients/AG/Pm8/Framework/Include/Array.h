// ****************************************************************************
//
//  File Name:			Array.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RArray template
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
//  $Logfile:: /PM8/Framework/Include/Array.h                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_ARRAY_H_
#define		_ARRAY_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

#define __AssertFile__ __FILE__

const	uLONG		kDefaultArrayGrowSize	= 64;

// ****************************************************************************
//
//  Class Name:		RArray
//
//  Description:	Template array class. Implements a linear memory array with
//						an extra always empty slot to designate the end of the array.
//						This slot can be used for temporary storage when shuffling
//						items in the array.
//
// ****************************************************************************
//

template <class T> class RArray
	{
	//	Iterator Class
	public :
		class RArrayIterator // : public RIterator
			{
				//	Member Data
				private :
					friend class RArray<T>;
					T*			m_pData;

				//	Construction & Destruction
				public :
										RArrayIterator( ) : m_pData(NULL)
											{
											;
											}
										RArrayIterator( const RArrayIterator& rhs ) : m_pData(rhs.m_pData)
											{
											;
											}
										RArrayIterator( T* pData ) : m_pData(pData)
											{
											;
											}
										~RArrayIterator( )
											{
											;
											}

				//	Typedefs and Structs
				public :
					typedef	T	YContainerType;

				//	Operator overloads
				public :
					//	Asignment operator
					RArrayIterator&	operator=( const RArrayIterator& rhs )
											{
											m_pData = rhs.m_pData;
											return *this;
											}
					//	Equality operators
					BOOLEAN			operator==(const RArrayIterator& rhs) const
											{
											return (BOOLEAN)(m_pData == rhs.m_pData);
											}
					BOOLEAN			operator!=(const RArrayIterator& rhs) const
											{
											return (BOOLEAN)(m_pData != rhs.m_pData);
											}
					BOOLEAN			operator<(const RArrayIterator& rhs) const
											{
											return (BOOLEAN)(m_pData < rhs.m_pData );
											}
					BOOLEAN			operator<=(const RArrayIterator& rhs) const
											{
											return (BOOLEAN)(m_pData <= rhs.m_pData );
											}
					BOOLEAN			operator>(const RArrayIterator& rhs) const
											{
											return (BOOLEAN)(m_pData > rhs.m_pData );
											}
					BOOLEAN			operator>=(const RArrayIterator& rhs) const
											{
											return (BOOLEAN)(m_pData >= rhs.m_pData );
											}
					//	Increment / Decrement operator
					RArrayIterator&	operator++( )			//	Prefix version
											{
											TpsAssert( m_pData, "Data pointer is NULL" );
											++m_pData;
											return *this;
											}
					RArrayIterator		operator++( int )		//	Postfix version
											{
											TpsAssert( m_pData, "Data pointer is NULL" );
											RArrayIterator	iterator = *this;
											++*this;
											return iterator;
											}
					RArrayIterator&	operator--( )			//	Prefix versino
											{
											TpsAssert( m_pData, "Data pointer is NULL" );
											--m_pData;
											return *this;
											}
					RArrayIterator		operator--( int )		//	Postfix version
											{
											TpsAssert( m_pData, "Data pointer is NULL" );
											RArrayIterator	iterator = *this;
											--*this;
											return iterator;
											}
					//	Addition / Subtraction operator
					RArrayIterator&	operator+=( int n )
											{
											TpsAssert( m_pData, "Data pointer is NULL" );
											m_pData	+= n;
											return *this;
											}
					RArrayIterator&	operator-=( int n )
											{
											TpsAssert( m_pData, "Data pointer is NULL" );
											m_pData	-= n;
											return *this;
											}
					RArrayIterator	operator+( int n )
											{
											TpsAssert( m_pData, "Data pointer is NULL" );
											RArrayIterator iter( *this );
											iter += n;
											return iter;
											}
					RArrayIterator	operator-( int n )
											{
											TpsAssert( m_pData, "Data pointer is NULL" );
											RArrayIterator iter( *this );
											iter -= n;
											return iter;
											}

					//	Dereference operators
					T					operator *( ) const
											{
											TpsAssert( m_pData, "Data pointer is NULL" );
											return *m_pData;
											}
					T&					operator *( )
											{
											TpsAssert( m_pData, "Data pointer is NULL" );
											return *m_pData;
											}
					T					operator []( int n ) const
											{
											TpsAssert( m_pData, "Data pointer is NULL" );
											return *( m_pData + n );
											}
					T&					operator []( int n )
											{
											TpsAssert( m_pData, "Data pointer is NULL" );
											return *( m_pData + n );
											}

			} ;


	// Typedefs
	public :
		typedef RArrayIterator YIterator;

	// Construction & Destruction
	public :
										RArray( uLONG ulGrowSize = kDefaultArrayGrowSize );
										RArray( T* pMemory, uLONG ulCount, uLONG ulGrowSize );
										RArray( const RArray<T>& rhs );
		virtual						~RArray( );

	// Operations
	public :
		YCounter						Count( ) const;
		const YIterator&			End( ) const;
		YIterator					Start( ) const;
		YIterator					Find( const T& ) const;

		void							InsertAt( YIterator& yPos, const T& );
		void							InsertAtStart( const T& );
		void							InsertAtEnd( const T& );

		void							RemoveAt( YIterator& yPos );
  		void							Remove( const T& );

		RArray<T>&					operator=( const RArray<T>& rhs );

		void							Copy( const RArray<T>& source );
		void							Empty( );
		void							UninitializedCopy( YIterator start, const YIterator& end, YIterator dst );
		void							Append( const YIterator& start, const YIterator& end );
		void							CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst );
		void							CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst );

										operator T*();
										operator const T*() const;

	//	Implementation
	private :
		void							QuickCopyItems( const YIterator& start, YIterator end, YIterator dst );
		void							QuickAppend( const YIterator& start, const YIterator& end );
		void							QuickEmpty( );
		void							QuickCopy( const RArray<T>& source );

	//	Member Data
	private :
		YIterator					m_FirstEntry;
		YIterator					m_LastEntry;
		YIterator					m_MaxEntry;
		uLONG							m_ulCount;
		uLONG							m_ulGrowSize;
	} ;


// ****************************************************************************
//		Redefine some of the handling for base type arrays
// ****************************************************************************
//
inline void RArray<sBYTE>::Copy( const RArray<sBYTE>& source )														{ QuickCopy( source ); }
inline void RArray<sBYTE>::Empty( )																							{ QuickEmpty( ); }
inline void	RArray<sBYTE>::UninitializedCopy( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }
inline void RArray<sBYTE>::Append( const YIterator& start, const YIterator& end )							{ QuickAppend( start, end ); }
inline void	RArray<sBYTE>::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )	{ QuickCopyItems( start, end, dst-(end.m_pData-start.m_pData) ); }
inline void	RArray<sBYTE>::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }

inline void RArray<uBYTE>::Copy( const RArray<uBYTE>& source )														{ QuickCopy( source ); }
inline void RArray<uBYTE>::Empty( )																							{ QuickEmpty( ); }
inline void	RArray<uBYTE>::UninitializedCopy( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }
inline void RArray<uBYTE>::Append( const YIterator& start, const YIterator& end )							{ QuickAppend( start, end ); }
inline void	RArray<uBYTE>::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )	{ QuickCopyItems( start, end, dst-(end.m_pData-start.m_pData) ); }
inline void	RArray<uBYTE>::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }

inline void RArray<sWORD>::Copy( const RArray<sWORD>& source )														{ QuickCopy( source ); }
inline void RArray<sWORD>::Empty( )																							{ QuickEmpty( ); }
inline void	RArray<sWORD>::UninitializedCopy( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }
inline void RArray<sWORD>::Append( const YIterator& start, const YIterator& end )							{ QuickAppend( start, end ); }
inline void	RArray<sWORD>::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )	{ QuickCopyItems( start, end, dst-(end.m_pData-start.m_pData) ); }
inline void	RArray<sWORD>::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }

inline void RArray<uWORD>::Copy( const RArray<uWORD>& source )														{ QuickCopy( source ); }
inline void RArray<uWORD>::Empty( )																							{ QuickEmpty( ); }
inline void	RArray<uWORD>::UninitializedCopy( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }
inline void RArray<uWORD>::Append( const YIterator& start, const YIterator& end )							{ QuickAppend( start, end ); }
inline void	RArray<uWORD>::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )	{ QuickCopyItems( start, end, dst-(end.m_pData-start.m_pData) ); }
inline void	RArray<uWORD>::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }

inline void RArray<sLONG>::Copy( const RArray<sLONG>& source )														{ QuickCopy( source ); }
inline void RArray<sLONG>::Empty( )																							{ QuickEmpty( ); }
inline void	RArray<sLONG>::UninitializedCopy( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }
inline void RArray<sLONG>::Append( const YIterator& start, const YIterator& end )							{ QuickAppend( start, end ); }
inline void	RArray<sLONG>::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )	{ QuickCopyItems( start, end, dst-(end.m_pData-start.m_pData) ); }
inline void	RArray<sLONG>::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }

inline void RArray<uLONG>::Copy( const RArray<uLONG>& source )														{ QuickCopy( source ); }
inline void RArray<uLONG>::Empty( )																							{ QuickEmpty( ); }
inline void	RArray<uLONG>::UninitializedCopy( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }
inline void RArray<uLONG>::Append( const YIterator& start, const YIterator& end )							{ QuickAppend( start, end ); }
inline void	RArray<uLONG>::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )	{ QuickCopyItems( start, end, dst-(end.m_pData-start.m_pData) ); }
inline void	RArray<uLONG>::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }

inline void RArray<int>::Copy( const RArray<int>& source )															{ QuickCopy( source ); }
inline void RArray<int>::Empty( )																							{ QuickEmpty( ); }
inline void	RArray<int>::UninitializedCopy( YIterator start, const YIterator& end, YIterator dst )		{ QuickCopyItems( start, end, dst ); }
inline void RArray<int>::Append( const YIterator& start, const YIterator& end )								{ QuickAppend( start, end ); }
inline void	RArray<int>::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )	{ QuickCopyItems( start, end, dst-(end.m_pData-start.m_pData) ); }
inline void	RArray<int>::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }

inline void RArray< RIntSize >::Copy( const RArray< RIntSize >& source )												{ QuickCopy( source ); }
inline void RArray< RIntSize >::Empty( )																							{ QuickEmpty( ); }
inline void	RArray< RIntSize >::UninitializedCopy( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }
inline void RArray< RIntSize >::Append( const YIterator& start, const YIterator& end )								{ QuickAppend( start, end ); }
inline void	RArray< RIntSize >::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )	{ QuickCopyItems( start, end, dst-(end.m_pData-start.m_pData) ); }
inline void	RArray< RIntSize >::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }

inline void RArray< RRealSize >::Copy( const RArray< RRealSize >& source )												{ QuickCopy( source ); }
inline void RArray< RRealSize >::Empty( )																							{ QuickEmpty( ); }
inline void	RArray< RRealSize >::UninitializedCopy( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }
inline void RArray< RRealSize >::Append( const YIterator& start, const YIterator& end )							{ QuickAppend( start, end ); }
inline void	RArray< RRealSize >::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )	{ QuickCopyItems( start, end, dst-(end.m_pData-start.m_pData) ); }
inline void	RArray< RRealSize >::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )	{ QuickCopyItems( start, end, dst ); }

inline void RArray< RIntPoint >::Copy( const RArray< RIntPoint >& source )													{ QuickCopy( source ); }
inline void RArray< RIntPoint >::Empty( )																								{ QuickEmpty( ); }
inline void	RArray< RIntPoint >::UninitializedCopy( YIterator start, const YIterator& end, YIterator dst )		{ QuickCopyItems( start, end, dst ); }
inline void RArray< RIntPoint >::Append( const YIterator& start, const YIterator& end )								{ QuickAppend( start, end ); }
inline void	RArray< RIntPoint >::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )		{ QuickCopyItems( start, end, dst-(end.m_pData-start.m_pData) ); }
inline void	RArray< RIntPoint >::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )		{ QuickCopyItems( start, end, dst ); }

inline void RArray< RRealPoint >::Copy( const RArray< RRealPoint >& source )												{ QuickCopy( source ); }
inline void RArray< RRealPoint >::Empty( )																							{ QuickEmpty( ); }
inline void	RArray< RRealPoint >::UninitializedCopy( YIterator start, const YIterator& end, YIterator dst )		{ QuickCopyItems( start, end, dst ); }
inline void RArray< RRealPoint >::Append( const YIterator& start, const YIterator& end )								{ QuickAppend( start, end ); }
inline void	RArray< RRealPoint >::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )	{ QuickCopyItems( start, end, dst-(end.m_pData-start.m_pData) ); }
inline void	RArray< RRealPoint >::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )		{ QuickCopyItems( start, end, dst ); }

// ****************************************************************************
//
//						Array Class
//
// ****************************************************************************
//

// ****************************************************************************
//
//  Function Name:	RArray<T>::RArray( )
//
//  Description:	Constructor
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline RArray<T>::RArray( uLONG ulGrowSize )
	: m_FirstEntry( NULL ),
	  m_LastEntry( NULL ),
	  m_MaxEntry( NULL ),
	  m_ulCount( 0 ),
	  m_ulGrowSize( ulGrowSize )
	{
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::RArray( )
//
//  Description:	Constructor
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline RArray<T>::RArray( T* pMemory, uLONG ulCount, uLONG ulGrowSize )
	: m_FirstEntry( pMemory ),
	  m_LastEntry( (pMemory + ulCount) ),
	  m_MaxEntry( m_LastEntry ),
	  m_ulCount( ulCount ),
	  m_ulGrowSize( ulGrowSize )
	{
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::RArray( )
//
//  Description:	Constructor
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline RArray<T>::RArray( const RArray<T>& rhs )
	: m_FirstEntry( NULL ),
	  m_LastEntry( NULL ),
	  m_MaxEntry( NULL ),
	  m_ulCount( 0 ),
	  m_ulGrowSize( rhs.m_ulGrowSize )
	{
	*this = rhs;
	}


// ****************************************************************************
//
//  Function Name:	RArray<T>::~RArray( )
//
//  Description:	Destructor
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline RArray<T>::~RArray( )
	{
	Empty( );
	::operator delete( m_FirstEntry.m_pData );
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::Count( )
//
//  Description:	Return the number of items in the list
//
//  Returns:		m_ulCount;
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline YCounter RArray<T>::Count( ) const
	{
	return m_ulCount;
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::Start( )
//
//  Description:	Return an iterator to the first item in the list.
//						The first item in the list is the Next of the m_Node.
//
//  Returns:		Iterator pointing to the first item in the list
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline RArray<T>::RArrayIterator RArray<T>::Start( ) const
	{
	return m_FirstEntry;
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::End( )
//
//  Description:	Return an iterator to the last item in the list.
//						The last item in the list is the m_Node.
//
//  Returns:		Iterator pointing to last item in the list
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline const RArray<T>::RArrayIterator& RArray<T>::End( ) const
	{
	return m_LastEntry;
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::Append( )
//
//  Description:	Append the given data onto this list
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline void RArray<T>::UninitializedCopy( YIterator start, const YIterator& end, YIterator dst )
	{
	::UninitializedCopy( start, end, dst );
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::Append( )
//
//  Description:	Append the given data onto this list
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline void RArray<T>::Append( const YIterator& start, const YIterator& end )
	{
	YIterator iterator = start;
	while ( iterator != end )
		{
		InsertAtEnd( *iterator );
		++iterator;
		}
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::CopyItemsBackwards( )
//
//  Description:	Copy the given items backwards from end to start
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline void RArray<T>::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )
	{
	::CopyItemsBackwards( end, dst, start );
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::CopyItemsForwards( )
//
//  Description:	Copy the given items forewards from start to end
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline void RArray<T>::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )
	{
	::CopyItemsForwards( start, end, dst );
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::Find( )
//
//  Description:	Return an iterator pointing AT the given item in the list
//
//  Returns:		Iterator pointing at the given item
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> RArray<T>::RArrayIterator RArray<T>::Find( const T& data ) const
	{
	YIterator iterator = Start( );
	for ( ; iterator != End(); ++iterator )
		{
		if (data == *iterator)
			break;
		}
	return iterator;
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::InsertAt( )
//
//  Description:	Insert an item at the given position
//
//  Returns:		Nothing
//
//  Exceptions:	Memory exception
//
// ****************************************************************************
//
template <class T> void RArray<T>::InsertAt( YIterator& yPos, const T& data )
	{
	TpsAssert( ((yPos >= m_FirstEntry) && (yPos <= m_LastEntry)), "Insert position is not in range" );
	//	Check if we have to grow the list
	//		if we don't, allocate the 'last' entry and copy backwards...
	if (m_LastEntry != m_MaxEntry)
		{
		//	If there is allocated memory, but no entries in the table
		if (m_FirstEntry == m_LastEntry)
			{
			new (m_LastEntry.m_pData) T( data );
			}
		else
			{
			YIterator	iter = m_LastEntry-1;
			new (m_LastEntry.m_pData) T( *iter );
			if (yPos != m_LastEntry)
				{
				CopyItemsBackwards( yPos, iter, m_LastEntry );
				}
			*yPos = data;
			}
		++m_LastEntry;
		}
	else
		{	//	we must allocate a new block of memory and copy everthing over
		T*				pNewT		= (T*)(::operator new( (uLONG)(m_ulCount + m_ulGrowSize)*sizeof(T) ) );
		YIterator	yStart( pNewT );
		YIterator	yTmp		= yStart;
		//	First, copy from m_FirstEntry to yPos
		if (yPos != m_FirstEntry)
			{
			UninitializedCopy( m_FirstEntry, yPos, yTmp );
			yTmp	+= (uLONG)(yPos.m_pData - m_FirstEntry.m_pData);
			}
		//	Copy the new item into the list
		new (yTmp.m_pData) T( data );
		++yTmp;
		// Copy any remaining entries
		if (yPos != m_LastEntry)
			{
			UninitializedCopy( yPos, m_LastEntry, yTmp );
			yTmp	+= (uLONG)(m_LastEntry.m_pData - yPos.m_pData);
			}
		//	Delete the original array items and array if it was ever allocated
		if (m_FirstEntry.m_pData)
			{
			for ( YIterator iterator = m_FirstEntry; iterator < m_LastEntry; ++iterator )
				iterator.m_pData->~T();
			::operator delete( m_FirstEntry.m_pData );
			}
		//	Setup list iterators to point to the start, end, and max.
		m_FirstEntry	= yStart;
		m_LastEntry		= yTmp;
		m_MaxEntry		= yStart + (m_ulCount + m_ulGrowSize);
		}

	//	item was inserted so increment count
	++m_ulCount;
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::InsertAtStart( )
//
//  Description:	Insert an item at the Start of the list
//
//  Returns:		Nothing
//
//  Exceptions:	Memory exception
//
// ****************************************************************************
//
template <class T> inline void RArray<T>::InsertAtStart( const T& data )
	{
	YIterator	iterator = Start();
	InsertAt( iterator, data );
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::InsertAtEnd( )
//
//  Description:	Insert an item at the End of the list
//
//  Returns:		Nothing
//
//  Exceptions:	Memory exception
//
// ****************************************************************************
//
template <class T> inline void RArray<T>::InsertAtEnd( const T& data )
	{
	YIterator	iterator = End();
	InsertAt( iterator, data );
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::RemoveAt( )
//
//  Description:	Remove an item at the given position
//
//  Returns:		Nothing
//
//  Exceptions:	Nothing
//
// ****************************************************************************
//
template <class T> void RArray<T>::RemoveAt( YIterator& yPos )
	{
	TpsAssert( ((yPos >= m_FirstEntry) && (yPos < m_LastEntry)), "Remove position is not in range" );

// metrowerks doesn't seem to like calling the destructor directly 
// this needs to be fixed when we do a mac version
// until then we can live with a small memory leak
#ifndef MAC
	yPos.m_pData->~T();
#endif

	if (yPos != (m_LastEntry-1))
		{
		YIterator	iter = yPos+1;
		CopyItemsForwards( iter, m_LastEntry, yPos );
		}
	
	--m_LastEntry;
	m_LastEntry.~RArrayIterator();
	--m_ulCount;
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::Remove( )
//
//  Description:	Remove the given item.
//
//  Returns:		Nothing
//
//  Exceptions:	
//
// ****************************************************************************
//
template <class T> void RArray<T>::Remove( const T& data )
	{
	TpsAssert( Count() != 0, "There are no items in this list" );
	YIterator	iterator = Find(data);
	if (iterator != m_LastEntry)
		RemoveAt( iterator );
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::Copy( )
//
//  Description:	Duplicate the given array
//
//  Returns:		Nothing
//
//  Exceptions:	kMemory;
//
// ****************************************************************************
//
template <class T> void RArray<T>::Copy( const RArray<T>& source )
	{
	//		First, remove all entries from the current array
	Empty( );
	//		For each entry in the source, Insert it into the array
	YIterator	iterator;
	for ( iterator = source.Start(); iterator != source.End(); ++iterator )
		InsertAtEnd( *iterator );
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::operator=( )
//
//  Description:	Copy the array
//
//  Returns:		Nothing
//
//  Exceptions:	kMemory;
//
// ****************************************************************************
//
template <class T> inline RArray<T>& RArray<T>::operator=( const RArray<T>& source )
	{
	if ( this != &source )
		Copy( source );
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::Empty( )
//
//  Description:	Remove All of the items in this array.
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> void RArray<T>::Empty( )
	{
	//	For each item in the list from back to front...
	YIterator	iterator = End( );
	while (Start( ) != iterator)
		{
		RemoveAt( --iterator );
		iterator = End( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::operator T*( )
//
//  Description:	Cast operator.
//
//  Returns:		Address of 1st item in the array
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> RArray<T>::operator T*()
	{
	return &( *Start() );
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::operator T*( )
//
//  Description:	Cast operator.
//
//  Returns:		Address of 1st item in the array
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> RArray<T>::operator const T*() const
	{
	return &( *Start() );
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::QuickCopyItems( )
//
//  Description:	Quickly copies the memory from start to dst given a length of
//							&end-&start;  It assumes the data has enough memory, and
//							that it does not have to adjust any internal pointers
//
//  Returns:		Nothing
//
//  Exceptions:	kMemory;
//
// ****************************************************************************
//
template <class T> inline void	RArray<T>::QuickCopyItems( const YIterator& start, YIterator end, YIterator dst )
	{
	uLONG		ulLength	= end.m_pData - start.m_pData;	//	number of bytes
	if ( ulLength )
		memmove( dst.m_pData, start.m_pData, ulLength*sizeof(T) );
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::QuickAppend( )
//
//  Description:	Quickly Append the data onto the end of the current data.
//						Verify there is enough room, call QuickCopyItems, and cleanup
//						internal pointers;
//
//  Returns:		Nothing
//
//  Exceptions:	kMemory;
//
// ****************************************************************************
//
template <class T> void	RArray<T>::QuickAppend( const YIterator& start, const YIterator& end )
	{
	//	Compute number of elements to append
	uLONG		ulNumElements	= end.m_pData - start.m_pData;	//	number of bytes
	//	Do Nothing if there is nothing to do.
	if ( ulNumElements == 0 )
		return;
	//
	//	Compute the number of elements that can fit into this array (without growing)
	uLONG		ulRemaining		= m_MaxEntry.m_pData - m_LastEntry.m_pData;

	//	If there is not enough room, grow the array
	if ( ulNumElements > ulRemaining )
		{
		//	Compute size and allocate new memory
		uLONG	ulNewSize	= ((m_ulCount + ulNumElements + m_ulGrowSize - 1) / m_ulGrowSize ) * m_ulGrowSize;
		T*	pNewT		= (T*)(::operator new( ulNewSize*sizeof(T) ) );
		//	copy data over
		YIterator	yStart( pNewT );
		QuickCopyItems( m_FirstEntry, m_LastEntry, yStart );
		//	Free old memory
		::operator delete( m_FirstEntry.m_pData );
		//	Reset pointers
		m_FirstEntry	= yStart;
		m_LastEntry		= yStart + m_ulCount;
		m_MaxEntry		= yStart + ulNewSize;
		}

	QuickCopyItems( start, end, m_LastEntry );
	m_LastEntry	+= ulNumElements;
	m_ulCount	+= ulNumElements;
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::QuickEmpty( )
//
//  Description:	Quickly remove all elements from this array
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline void	RArray<T>::QuickEmpty( )
	{
	m_LastEntry	= m_FirstEntry;
	m_ulCount	= 0;
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::QuickCopy( )
//
//  Description:	Performs a quick duplication of the source array
//
//  Returns:		Nothing
//
//  Exceptions:	kMemory;
//
// ****************************************************************************
//
template <class T> inline void RArray<T>::QuickCopy( const RArray<T>& source )
	{
	//		First, remove all entries from the current array
	QuickEmpty( );
	QuickAppend( source.Start(), source.End() ); 

#ifdef	TPSDEBUG
	TpsAssert( m_ulCount == source.m_ulCount, "Counts do not match" );
	YIterator	iter1( Start() );
	YIterator	iterEnd( End() );
	YIterator	iter2( source.Start() );
	for ( ; iter1 != iterEnd; ++iter1, ++iter2 )
		TpsAssert( *iter1 == *iter2, "Quick Copy Did Not" );
#endif
	}

#undef __AssertFile__


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_ARRAY_H_
