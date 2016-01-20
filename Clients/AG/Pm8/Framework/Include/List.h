// ****************************************************************************
//
//  File Name:			List.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RList class
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
//  $Logfile:: /PM8/Framework/Include/List.h                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_LIST_H_
#define		_LIST_H_

#define __AssertFile__ __FILE__

// ****************************************************************************
//
//  Class Name:	RList
//
//  Description:	Template list class. Implements a circular doubly linked
//						list. This list maintains a End iterator inside of itself for
//						the ability to determine when the list is at the end.
//
// ****************************************************************************
//
template <class T> class RList
	{
	//	Private structures and members
	private :
		class	RListNode
			{
				//	Member data
				public :
					RListNode*		m_pPrevNode;
					RListNode*		m_pNextNode;
#ifdef	TPSDEBUG
					RListNode*		m_pEndNode;
#endif	//	TPSDEBUG

					T					m_Data;

				//	Constrction & Destruction
				public :
										RListNode( )
											: m_pPrevNode(NULL),
											  m_pNextNode(NULL),
#ifdef	TPSDEBUG
											  m_pEndNode(NULL),
#endif	//	TPSDEBUG
											  m_Data()
											{
											;
											}


										RListNode( const T& data )
											: m_pPrevNode(NULL),
											  m_pNextNode(NULL),
#ifdef	TPSDEBUG
											  m_pEndNode(NULL),
#endif	//	TPSDEBUG
											  m_Data(data)
											{
											;
											}

										~RListNode( )
											{
											;
											}

										//
										//		Add the given node in front of this node
				void					Link( RListNode* pNode )
											{
											TpsAssert( pNode,	"Linking NULL into the list" );
											TpsAssert( m_pPrevNode, "This node has no previous node pointer" );
											TpsAssert( m_pNextNode, "This node has no next node pointer" );
												//	Previous node of new node points previous of given
											pNode->m_pPrevNode	= m_pPrevNode;
												//	Next node of new node points to given
											pNode->m_pNextNode	= this;
											//	adjust the pointers of the nodes in the list
												//	Next node of previous node points to new node
											m_pPrevNode->m_pNextNode = pNode;
												//	previous node of given node points to new node
											m_pPrevNode = pNode;

				#ifdef	TPSDEBUG
											TpsAssert( m_pEndNode != NULL, "List Iterator End Node is NULL?" );
												//	Copy the end node reference
											pNode->m_pEndNode = m_pEndNode;
				#endif	//	TPSDEBUG
											}

										//
										//		Remove this node from being linked to the other nodes
				void					Unlink( )
											{
											TpsAssert( m_pPrevNode, "This node has no previous node pointer" );
											TpsAssert( m_pNextNode, "This node has no next node pointer" );
												//	the previous nodes next node should point to this nodes next node
											m_pPrevNode->m_pNextNode = m_pNextNode;
												//	the next nodes previous node should point to this nodes previous node
											m_pNextNode->m_pPrevNode = m_pPrevNode;
											}
		
			} ;

	//	Iterator Class
	public :
		class RListIterator // : public RIterator
			{
				//	Member Data
				private :
					friend class RList<T>;
					RListNode*		m_pNode;

				//	Construction & Destruction
				public :
										RListIterator( )
											: m_pNode(NULL)
											{
											;
											}
										RListIterator( const RListIterator& rhs )
											: m_pNode(rhs.m_pNode)
											{
											;
											}
										RListIterator( RListNode* pNode ) : m_pNode(pNode)
											{
											;
											}
										~RListIterator( )
											{
											;
											}

				//	Typedefs and Structs
				public :
					typedef	T	YContainerType;

				//	Operator overloads
				public :
					//	Asignment operator
					RListIterator&	operator=( const RListIterator& rhs )
											{
											m_pNode = rhs.m_pNode;
											return *this;
											}
					//	Equality operator
					BOOLEAN			operator==(const RListIterator& rhs) const
											{
											TpsAssert( m_pNode, "Node is NULL" );
											return (BOOLEAN)(m_pNode == rhs.m_pNode);
											}
					BOOLEAN			operator!=(const RListIterator& rhs) const
											{
											TpsAssert( m_pNode, "Node is NULL" );
											return (BOOLEAN)(m_pNode != rhs.m_pNode);
											}
					//	Dereference operator
					T					operator *( ) const
											{
											TpsAssert( m_pNode, "Node is NULL" );
											TpsAssert( m_pNode != m_pNode->m_pEndNode, "Dereferencing the End Node" );
											return m_pNode->m_Data;
											}
					T&					operator *( )
											{
											TpsAssert( m_pNode, "Node is NULL" );
											TpsAssert( m_pNode != m_pNode->m_pEndNode, "Dereferencing the End Node" );
											return m_pNode->m_Data;
											}
					T					operator []( int n ) const
											{
											RListIterator iter = *this;
											iter += n;
											return *iter;
											}
					T&					operator []( int n )
											{
											RListIterator iter = *this;
											iter += n;
											return *iter;
											}
					//	Increment operator
					RListIterator&	operator++( )		//	Prefix notation
											{
											TpsAssert( m_pNode, "Node is NULL" );
											m_pNode = m_pNode->m_pNextNode;
											return *this;
											}
					RListIterator	operator++( int )		//	Postfix notation
											{
											TpsAssert( m_pNode, "Node is NULL" );
											RListIterator	iterator = *this;
											++*this;
											return iterator;
											}
					//	Addition / Subtraction operator
					RListIterator&	operator+=( int n )
											{
											if (n < 0)
												*this -= (-n);
											else
												{
												while (n-- > 0)
													{
													TpsAssert( m_pNode, "Node is NULL" );
													m_pNode = m_pNode->m_pNextNode;
													}
												}
											return *this;
											}
					RListIterator&	operator-=( int n )
											{
											TpsAssert( m_pNode, "Node is NULL" );
											if (n < 0)
												*this += (-n);
											else
												{
												while (n-- > 0)
													{
													TpsAssert( m_pNode, "Node is NULL" );
													m_pNode = m_pNode->m_pPrevNode;
													}
												}
											return *this;
											}
					RListIterator	operator+( int n )
											{
											RListIterator iter( *this );
											iter += n;
											return iter;
											}
					RListIterator	operator-( int n )
											{
											RListIterator iter( *this );
											iter -= n;
											return iter;
											}
					//	Decrement operator
					RListIterator&	operator--( )			//	Prefix notation
											{
											TpsAssert( m_pNode, "Node is NULL" );
											m_pNode = m_pNode->m_pPrevNode;
											return *this;
											}
					RListIterator	operator--( int )			//	Posfix notation
											{
											TpsAssert( m_pNode, "Node is NULL" );
											RListIterator	iterator = *this;
											--*this;
											return iterator;
											}

			} ;


	// Typedefs
	public :
		typedef RListIterator YIterator;

	// Construction & Destruction
	public :
										RList( );
										RList( const RList<T>& rhs );
		virtual						~RList( );

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

		RList<T>&					operator=( const RList<T>& rhs );
		void							Copy( const RList<T>& source );
		void							Empty( );
		void							Append( const YIterator& start, const YIterator& end );
		void							CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst );
		void							CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst );

	//	Member Data
	private :
		RListNode					m_Node;
		YIterator					m_EndIterator;
		uLONG							m_ulCount;
	} ;

// ****************************************************************************
//
//						List Class
//
// ****************************************************************************
//

// ****************************************************************************
//
//  Function Name:	RList<T>::RList( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline RList<T>::RList( )
	: m_Node( ),
	  m_EndIterator( &m_Node ),
	  m_ulCount( 0 )
	{
	m_Node.m_pNextNode	= &m_Node;
	m_Node.m_pPrevNode	= &m_Node;
#ifdef	TPSDEBUG
	m_Node.m_pEndNode		= &m_Node;
#endif	//	TPSDEBUG	
	}

// ****************************************************************************
//
//  Function Name:	RList<T>::RList( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline RList<T>::RList( const RList<T>& rhs )
	: m_Node( ),
	  m_EndIterator( &m_Node ),
	  m_ulCount( 0 )
	{
	m_Node.m_pNextNode	= &m_Node;
	m_Node.m_pPrevNode	= &m_Node;
#ifdef	TPSDEBUG
	m_Node.m_pEndNode		= &m_Node;
#endif	//	TPSDEBUG	

	*this = rhs;
	}

// ****************************************************************************
//
//  Function Name:	RList<T>::End( )
//
//  Description:		Return an iterator to the last item in the list.
//							The last item in the list is the m_Node.
//
//							Note: This function is out of order because on some
//							compiliers (Ohh, MetroWerks for instance) an function
//							cannot be declaired as inline after it is referenced.
//
//  Returns:			Iterator pointing to last item in the list
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline const RList<T>::RListIterator& RList<T>::End( ) const
	{
	return m_EndIterator;
	}

// ****************************************************************************
//
//  Function Name:	RList<T>::Start( )
//
//  Description:		Return an iterator to the first item in the list.
//							The first item in the list is the Next of the m_Node.
//
//							Note: This function is out of order because on some
//							compiliers (Ohh, MetroWerks for instance) an function
//							cannot be declaired as inline after it is referenced.
//
//  Returns:			Iterator pointing to the first item in the list
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline RList<T>::RListIterator RList<T>::Start( ) const
	{
	YIterator	start	= End( );
	return ++start;
	}

// ****************************************************************************
//
//  Function Name:	RList<T>::RemoveAt( )
//
//  Description:		Remove an item at the given position
//
//							Note: This function is out of order because on some
//							compiliers (Ohh, MetroWerks for instance) an function
//							cannot be declaired as inline after it is referenced.
//
//  Returns:			Nothing
//							
//  Exceptions:		Nothing
//
// ****************************************************************************
//
template <class T> inline void RList<T>::RemoveAt( YIterator& yPos )
	{
	TpsAssert( yPos != End(), "Trying to remove the End entry. This is bad");
	TpsAssert( yPos.m_pNode, "Given position to remove has no Node" );

	//	Unlink the node and then delete it
	yPos.m_pNode->Unlink();
	delete yPos.m_pNode;
	--m_ulCount;
	}

// ****************************************************************************
//
//  Function Name:	RList<T>::~RList( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline RList<T>::~RList( )
	{
	Empty( );
	}

// ****************************************************************************
//
//  Function Name:	RList<T>::Count( )
//
//  Description:		Return the number of items in the list
//
//  Returns:			m_ulCount;
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline YCounter RList<T>::Count( ) const
	{
	return m_ulCount;
	}

// ****************************************************************************
//
//  Function Name:	RList<T>::Find( )
//
//  Description:		Return an iterator pointing AT the given item in the list
//
//  Returns:			Iterator pointing at the given item
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline RList<T>::RListIterator RList<T>::Find( const T& data ) const
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
//  Function Name:	RList<T>::InsertAt( )
//
//  Description:		Insert an item at the given position
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
template <class T> inline void RList<T>::InsertAt( YIterator& yPos, const T& data )
	{
  	//	allocate and link the new node
	RListNode*	pNode = new RListNode( data );
	yPos.m_pNode->Link( pNode );
	//	increment count
	++m_ulCount;
	}

// ****************************************************************************
//
//  Function Name:	RList<T>::InsertAtStart( )
//
//  Description:		Insert an item at the Start of the list
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
template <class T> inline void RList<T>::InsertAtStart( const T& data )
	{
	YIterator	iterator = Start();
	InsertAt( iterator, data );
	}

// ****************************************************************************
//
//  Function Name:	RList<T>::InsertAtEnd( )
//
//  Description:		Insert an item at the End of the list
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
template <class T> inline void RList<T>::InsertAtEnd( const T& data )
	{
	YIterator	iterator = End();
	InsertAt( iterator, data );
	}

// ****************************************************************************
//
//  Function Name:	RList<T>::Remove( )
//
//  Description:		Remove the given item.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> inline void RList<T>::Remove( const T& data )
	{
	TpsAssert( Count() != 0, "There are no items in this list" );
	YIterator	iterator = Find(data);
	if (iterator != End())
		RemoveAt( iterator );
	}

// ****************************************************************************
//
//  Function Name:	RArray<T>::Copy( )
//
//  Description:	Remove the given item.
//
//  Returns:		Nothing
//
//  Exceptions:	kMemory
//
// ****************************************************************************
//
template <class T> void RList<T>::Copy( const RList<T>& source )
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
//  Description:	Assign the array to be a copy of the rhs.
//
//  Returns:		reference to this
//
//  Exceptions:	kMemory
//
// ****************************************************************************
//
template <class T> inline RList<T>& RList<T>::operator=( const RList<T>& source )
	{
	Copy( source );
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RList<T>::Empty( )
//
//  Description:	Remove All of the items in this container.
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> void RList<T>::Empty( )
	{
	YIterator	iterator = End( );
	while (Start( ) != iterator)
		{
		RemoveAt( --iterator );
		iterator = End( );
		}
	}


// ****************************************************************************
//
//  Function Name:	RList<T>::Append( )
//
//  Description:	Append the given data onto this list
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline void RList<T>::Append( const YIterator& start, const YIterator& end )
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
//  Function Name:	RList<T>::CopyItemsBackwards( )
//
//  Description:	Copy the given items backwards from end to start
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline void RList<T>::CopyItemsBackwards( const YIterator& start, YIterator end, YIterator dst )
	{
	::CopyItemsBackwards( end, dst, start );
	}

// ****************************************************************************
//
//  Function Name:	RList<T>::CopyItemsForwards( )
//
//  Description:	Copy the given items forewards from start to end
//
//  Returns:		Nothing
//
//  Exceptions:	None
//
// ****************************************************************************
//
template <class T> inline void RList<T>::CopyItemsForwards( YIterator start, const YIterator& end, YIterator dst )
	{
	::CopyItemsForwards( start, end, dst );
	}

#undef __AssertFile__

#endif	//	_LIST_H_
