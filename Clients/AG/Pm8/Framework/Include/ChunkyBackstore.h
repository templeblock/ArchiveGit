// ****************************************************************************

#ifndef		_CHUNKY_BACKSTORE_H_
#define		_CHUNKY_BACKSTORE_H_

#ifndef		_CHUNKY_STORAGE_H_
#include		"ChunkyStorage.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RChunkStorage;

// ****************************************************************************
//
// Class Name:			RChunkBackstore
//
// Description:		Support for persistant runtime storage of objects
//
// ****************************************************************************
//
class FrameworkLinkage RChunkBackstore : public RChunkStorage
	{
	// Construction, Destruction & Initialization
	public :
										RChunkBackstore( );
										RChunkBackstore( BOOLEAN fMemoryBackstore );
										~RChunkBackstore( );

		virtual void				Initialize( );

	// Operations
	public :
		void							Remove( const YChunkTag yChunkTag, 
											const YChunkId yChunkId );

		void							CleanupTable( );
		void							Copy( const YChunkInfo& yci, YStreamLength yNewSize );

	// Implementation
	protected :
		virtual void				Grow( );

	// Debugging Support
#ifdef TPSDEBUG
	// Debugging Support
	public :
		virtual void				Validate( ) const;
#endif

	// Member data
	private :
	};


// ****************************************************************************
// 					Inlines
// ****************************************************************************


// ****************************************************************************
//
//  Function Name:	RChunkBackstore::RChunkBackstore( )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RChunkBackstore::RChunkBackstore( )
	: RChunkStorage( (BOOLEAN)!kMemoryStream )
	{
	NULL;
	}


// ****************************************************************************
//
//  Function Name:	RChunkBackstore::RChunkBackstore( BOOLEAN fMemoryBackstore )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RChunkBackstore::RChunkBackstore( BOOLEAN fMemoryBackstore )
	: RChunkStorage( fMemoryBackstore )
	{
	NULL;
	}


// ****************************************************************************
//
//  Function Name:	RChunkBackstore::~RChunkBackstore( )
//
//  Description:		dtor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RChunkBackstore::~RChunkBackstore( )
	{
	NULL;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//_CHUNKY_BACKSTORE_H_