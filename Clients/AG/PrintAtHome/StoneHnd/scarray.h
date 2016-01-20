#pragma once

#define kInitBlockSize 16

/* ==================================================================== */
template <class T>
class scArray
{
public:

	scArray()
	{
		elemSlots_ = kInitBlockSize;
		blockSize_ = kInitBlockSize;
		numItems_ = 0;
		items_ = (T*)malloc( kInitBlockSize * sizeof( T ) );
		retainMem_ = true;

		ClearMem( 0 );
	}

	~scArray()
	{
		for ( int i = 0; i < numItems_; i++)
			deleteItem( i );

		numItems_ = 0;
		if ( items_ )
		{
			free( items_ );
			items_ = NULL;
		}
	}
						
	int NumItems() const
	{
		return numItems_;
	}
	
	T& operator[]( int n )
	{
		return items_[ n ];
	}

	const T& operator[]( int n ) const
	{
		return items_[ n ];
	}

	T* ptr()
	{
		return items_;
	}

	const T* ptr() const
	{
		return items_;
	}

	void Remove( int index )
	{
		scAssert( index < numItems_ );
		deleteItem( index );

		if ( numItems_ - index - 1 )
			memmove( items_ +index, items_ +index + 1,
						( numItems_ - index - 1 ) * sizeof( T )  );

		numItems_ -= 1;

		ShrinkSlots();
	}

	void RemoveAll()
	{
		for ( int i = 0; i < numItems_; i++)
			deleteItem( i );

		numItems_ = 0;
		ShrinkSlots();
	}

	T& Grow()
	{
		SetNumSlots( numItems_ + 1 );
		return items_[numItems_++];
	}
	
	int Append( const T& elem )
	{
		SetNumSlots( numItems_ + 1 );
		items_[numItems_] = elem;
		return numItems_++;
	}

	void Insert( int index, const T& elem )
	{
		SetNumSlots( numItems_ + 1 );

		if ( numItems_ - index > 0 ) 
			memmove( items_ + index + 1, items_ + index,
					( numItems_ - index ) * sizeof(T) );

		constructItem( index );
		items_[index] = elem;	
		numItems_++;
	}

	void Set( int index, const T& elem )
	{
		if ( index >= numItems_ ) {
			SetNumSlots( index + 1 );
			numItems_ = index + 1;
		}
		
		items_[index] = elem;	
	}

	void SetNumSlots( int numSlots )
	{
		int numBlocks = MAX( 1, numSlots / blockSize_ + ( numSlots % blockSize_ ? 1 : 0 ) );
		SizeSlots( numBlocks * blockSize_ );
	}

	void SetRetainMem( bool tf )
	{
		retainMem_ = tf ? true : false;
	}

	bool GetRetainMem() const
	{
		return retainMem_;
	}

private:
	void constructItem( int index )
	{
		memset(&items_[index], 0, sizeof(T));
	}

	void deleteItem( int index )
	{
		items_[index].T::~T();
	}

	void MoreSlots()
	{
		GrowSlots( blockSize_ );
	}

	void ShrinkSlots()
	{
		SetNumSlots( numItems_ );
	}
	
	void SizeSlots( int numItems )
	{
		// do not shrink if we are retaining memory or if no resizing is necessary
		//
		if ( elemSlots_ == numItems || ( numItems < elemSlots_ && retainMem_ ) )
			return;

		scAssert( numItems >= blockSize_ );

		long oldSize = elemSlots_;

		if ( items_ == 0 )
			throw( -1 ); 

		items_ = (T*)realloc( items_, sizeof(T) * numItems );
		elemSlots_ = numItems;
		ClearMem( oldSize );
	}

	void GrowSlots( int newItems )
	{
		int oldSize = elemSlots_;

		if ( items_ == 0 )
			throw( -1 );

		items_ = (T*)realloc( items_, sizeof(T) * ( elemSlots_ + newItems ) );
		elemSlots_ += newItems;
		ClearMem( oldSize );
	}

	void ClearMem( int oldsize )
	{
		// either we do need to clear memory or we have shrunk it
		if ( oldsize >= elemSlots_ )
			return;

		for ( int index = oldsize; index < elemSlots_; index++ )
			constructItem( index ); 	
	}

private:
	T* items_;
	int numItems_;
	int elemSlots_;		// num of elements potentially in allocated space
	int blockSize_;		// for growing and shrinking we grow in greater than one element unit
	bool retainMem_; 	// do not shrink memory if this is set
};
