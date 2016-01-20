//		FixedSizeAllocator
//
//		Inline file for handling a fixed size object allocator
//

const int kAllocationSize	= 1024;	//	Number of objects to store in a single buffer

//
// ===========================================================
//
//		FixedSizeBuffer
//
class FixedSizeBuffer
{
private:
	void*	m_pFirstObject;			//	Address of first object in buffer
	void*	m_pLastObject;				//	Address of last valid object in buffer
	void*	m_pFirstFree;				//	First Free in this buffer
	int	m_nInUseCount;				//	Number if items used in buffer

	FixedSizeBuffer*	m_pNextBuffer;	//	next object buffer

public:
							FixedSizeBuffer( size_t t );
							~FixedSizeBuffer( ) ;

	//
	//	Accessors
	FixedSizeBuffer*	GetNextBuffer( )	{ return m_pNextBuffer;	}

	//
	//	Implementation
	void*					GetAllocation( size_t t );
	bool					FreeAllocation( void* pData );

private :
	//		IsInBuffer - Returns true if pdata lies inside of buffer range
	bool					IsInBuffer( void* pData )		{ return static_cast<bool>((m_pFirstObject<=pData)&&(pData<=m_pLastObject)); }
};

//
//	@MFunc	Constructor
//	@RDesc	Object constructor, may throw a memory exception
inline FixedSizeBuffer::FixedSizeBuffer( size_t t )	//	@Parm	size of the individual object in the buffer
	: m_pFirstObject( NULL ), 
	  m_pLastObject( NULL ), 
	  m_pNextBuffer( NULL ), 
	  m_pFirstFree( NULL ), 
	  m_nInUseCount( 0 )
{
	const	int	kBufferSize	= t*kAllocationSize;

	TpsAssert( t >= sizeof(void*), "Object size to small for Object Allocator" );
	m_pFirstObject = ::operator new( kBufferSize );
	m_pFirstFree	= m_pFirstObject;
	m_pLastObject	= ((char*)m_pFirstObject) + kBufferSize - t;
	//	Setup free list;
	void** pData = (void**)m_pFirstFree;
	for( ; pData < (void**)m_pLastObject; pData = (void**)(((char*)pData) + t) )
		*pData = ((char*)pData) + t;
	*pData	= NULL;
}

//
//	@MFunc	Destructor
//	@RDesc	Nothing
inline FixedSizeBuffer::~FixedSizeBuffer( )
{
	::operator delete( m_pFirstObject );	//	Delete the object data
}

//
//	@MFunc	Get Allocation to be used for the upcomming object creation
//	@RDesc	pointer for memory that is 'Free'
inline void* FixedSizeBuffer::GetAllocation( size_t t )
{
	void*	pAllocation	= NULL;

	//	If we have a valid allocation in here, use it
	if ( m_pFirstFree )
		{
		//	Use first free in this block
		pAllocation		= m_pFirstFree;
		//	Adjust first free pointer by storing reference to contents value
		//	which is the next free frame
		m_pFirstFree	= *(void**)m_pFirstFree;
		//	Using one more from this block
		++m_nInUseCount;
		}
	else
		{
		//	If there is no next block, make it
		if ( m_pNextBuffer == NULL )
			m_pNextBuffer = new FixedSizeBuffer(t);

		//	Ask next block for allocation
		TpsAssert( m_pNextBuffer != NULL, "Next buffer is NULL.  Why wasn't it created?" );
		pAllocation = m_pNextBuffer->GetAllocation(t);
		}

	//	Return value
	return pAllocation;
}

//
//	@MFunc	FreeAllocation - mark this data as being usable for the next allocation
//	@RDesc	TRUE if the buffer is now empty, FALSE if still being used
inline bool FixedSizeBuffer::FreeAllocation( void* pData )
{
	if ( !IsInBuffer( pData ) )
	{
		TpsAssert( m_pNextBuffer, "Data not found in any active buffer" );
		if ( m_pNextBuffer->FreeAllocation( pData ) )
		{
			FixedSizeBuffer*	pOld	= m_pNextBuffer;
			m_pNextBuffer = m_pNextBuffer->GetNextBuffer( );
			delete pOld;
		}
	}
	//
	//	Allocation is inside of this buffer
	else if ( --m_nInUseCount > 0 )
	{
		//	Remember the current free data in this pointers location
		*((void**)pData)	= m_pFirstFree;
		m_pFirstFree		= pData;
	}

	//	Return inUseCount so caller can free buffer if it is now empty
	return static_cast<bool>(m_nInUseCount == 0);
}

//
//	===========================================================
//
//		FixedSizeAllocator
//
//			Template class to handle the allocation of fixed size objects
//		through the FixedSizeBuffer
//
template< class T > class FixedSizeAllocator
{
private:

private:
	FixedSizeBuffer*				m_pHeadBuffer;			//	Pointer to first object buffer
	void*								m_pFirstAllocation;

public:
	FixedSizeAllocator( );
	~FixedSizeAllocator( );

	void*	AllocateObject( );
	void  FreeObject( void* pBuffer );

} ;

template < class T >
inline FixedSizeAllocator<T>::FixedSizeAllocator( )
 : m_pHeadBuffer( NULL ),
 	m_pFirstAllocation( NULL )
{
	m_pFirstAllocation = AllocateObject( );
}

template < class T >
inline FixedSizeAllocator<T>::~FixedSizeAllocator( )
{
	FreeObject( m_pFirstAllocation );
	TpsAssert( m_pHeadBuffer == NULL,"Object buffer being objects still in use, this is a memory leak" );
}

template < class T >
inline void* FixedSizeAllocator<T>::AllocateObject( )
{
	if ( m_pHeadBuffer == NULL )
		m_pHeadBuffer	= new FixedSizeBuffer( sizeof(T) );
	return m_pHeadBuffer->GetAllocation( sizeof(T) );
}

template < class T >
inline void FixedSizeAllocator<T>::FreeObject( void* pObject )
{
	if ( pObject )
		{
		TpsAssert( m_pHeadBuffer, "Freeing object but no Head buffer exists" );
		if ( m_pHeadBuffer->FreeAllocation( pObject ) )
			{
			FixedSizeBuffer*	pOld	= m_pHeadBuffer;
			m_pHeadBuffer = m_pHeadBuffer->GetNextBuffer( );
			delete pOld;
			}
		}
}
