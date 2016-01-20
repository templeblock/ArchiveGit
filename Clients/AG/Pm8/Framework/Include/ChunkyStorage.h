// ****************************************************************************
//
//  File Name:			ChunkStorage.h
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RChunkStorage class
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
//  $Logfile:: /PM8/Framework/Include/ChunkyStorage.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CHUNKY_STORAGE_H_
#define		_CHUNKY_STORAGE_H_

#ifndef		_STORAGE_H_
#include		"Storage.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//	Forwards
class RChunkStorage;
class RChunkBackstore;
class RChunkSearcher;

//	enumerated defns
enum EProductTag	{ kRenaissance='PSD5' };
enum EChunkType	{ kNoType='EMTY'					//	uninitialized chunk
						, kRenaissanceDoc='RDOC'		//	documment
						, kGraphicLibrary='GRLB'		//	graphic library
						, kAuxData='AUXD'					//	aux data
						};

typedef EProductTag	YProductTag;
typedef EChunkType	YChunkType;

//	symbolic arguments
const BOOLEAN kRecursive =					TRUE;	//	make chunk search continue to children
const BOOLEAN kSearchTag =					TRUE;	//	make chunk search based on given tag
const BOOLEAN kSearchVersion =			TRUE;	//	make chunk search based on given version
const BOOLEAN kSearchId =					TRUE;	//	make chunk search based on given id
const BOOLEAN kFixMaxSize =				TRUE;	//	keep chunk from growing in its original stream
const BOOLEAN kGetRoot =					TRUE;	//	return root storage from GetFirstChunk
const BOOLEAN kMemoryStream = 			TRUE;	//	use a memory stream to back a backstore
const BOOLEAN kFlushStream =				TRUE;	//	cause the stream backing the storage to be flushed
const BOOLEAN kFirstWrite =				TRUE;	//	make WriteHeader add the kWriteIncomplete flag to the disk header
const BOOLEAN kPlaceholder =				TRUE;	//	make a storage that is only used for keeping the user from deleting our doc while we have it open
const BOOLEAN kCreateBackstoreIfNecessary =	TRUE;	//	Allow GetRootBackstore to create the backstore if it does not exist
const YChunkTag kTagDontCare =			'DNCR';	//	symbolic name for a tag argument we dont care about. THIS IS A VALID TAG. THE CHUNK STORAGE SYSTEM WILL TREAT IT LIKE ANY OTHER TAG.
const YChunkId kIdDontCare =				0;	//	symbolic name for an id argument we dont care about. THIS IS A VALID ID. THE CHUNK STORAGE SYSTEM WILL TREAT IT LIKE ANY OTHER ID.
const BOOLEAN kForceUpdate =				TRUE;	//	As an argument to ReadChunkIndex, force the chunk's indices and header to be read from disk.

//	chunk root flags
const uLONG kNoRootFlags =					0UL;	//	when the chunk is deconstructed write its index
const uLONG kWriteClosingIndex =			1UL;	//	when the chunk is deconstructed write its index
const uLONG kDeleteStorageOnClose =		2UL;	//	when the chunk is deconstructed delete its storage object (not its disk file)
const uLONG kWriteAuxDataOnClose =		4UL;	//	when the chunk is deconstructed write its aux data
const uLONG kIndexWritten =				8UL;	//	the chunk's index has been written
const uLONG kReplaceExisting =			16UL;	//	when the chunk tree is destructed safe save over the original file
const uLONG kOpenedExisting =				32UL;	//	the stream backing the storage existed when the chunk tree was constructed
const uLONG kChunkDirty =					64UL;	//	the chunk tree needs to be saved since it differs from its disk copy
const uLONG kWriteIncomplete =			128UL;//	the chunk tree has not finished writing yet and is invalid
const uLONG kDeleteOnClose =				256UL;// this tree is for temporary use only and should be deleted when closed

//	chunk index flags
const uLONG kDeleteChunkPtrOnClose =	512UL;//	when the parent index is destructed delete the child chunk it points to since we created it
const uLONG kChunkContiguousDataDone =	1024UL;// no more contiguous local data can be written in the chunk; the data is surrounded by child chunk's
const uLONG kChunkDeleted =				2048UL;// no more contiguous local data can be written in the chunk; the data is surrounded by child chunk's

const uLONG kChunkAlign =					sizeof(uLONG);	//	byte alignment for start of chunks

//	values for YFlags
const uLONG kNoFlags =						0x00000000UL;	//	no flags set
const uLONG kSubChunk =						0x00000001UL;	//	the chunk contains a chunk

const uLONG kInvalidChunkIndex =			0xFFFFFFFFUL;	//	uninitialized chunk index
const uLONG kInvalidChunkPos =			0xFFFFFFFFUL;	//	uninitialized chunk position
const uLONG kRootTag =						'ROOT';			//	tag of root chunk (which has no parent to contain its index)
const uLONG kSubTreeRootTag =				'STRT';			//	tag of sub tree root chunk used when a tree in created from an arbitrary storage without context
const uLONG kComponent =					'COMP';			//	tag of root chunk (which has no parent to contain its index)
const uLONG kMaxChunkSize =				ULONG_MAX;		//	largest size a chunk may be
const uLONG kExtraFill =					'****';			// fill end of alignment padding at end of chunk with this pattern
const uLONG kMagicKey =						0xA679;			// special value that identifies a structure as a chunky header

//	Utility fcns
void ToggleFlag( YFlags& yFlagStorage, const YFlags yFlag );
void ClearFlag( YFlags& yFlagStorage, const YFlags yFlag );
void SetFlag( YFlags& yFlagStorage, const YFlags yFlag );
BOOLEAN FlagIsSet( YFlags yFlagStorage, const YFlags yFlag );


// ****************************************************************************
//
// Class Name:			YChunkHeader
//
// Description:		holds location and content description of chunk index and data
//
// ****************************************************************************
//
class FrameworkLinkage YChunkHeader
	{
	// Construction, Destruction & Initialization
	public :
										YChunkHeader( );
	public:
		//	NOTE:	Considerable caution must be used when modifying this structure.
		//	Do not insert fields before the note below. Do not change the order of these fields. This will allow us to
		//	read old versions of chunk storages after fields have been added to 
		// this structure in future versions.
		
		uLONG							ulMagicKey;						//	special value that identifies this structure as a chunky header
		YPlatformTag				yPlatformTag;					//	native platform for this chunk 
		YHeaderSize					yHeaderSize;					//	sizeof the header as written when the header was created; may grow in the future
		YProductTag					yProductTag;					//	kind of app that originated the format of this chunk
		YChunkType					yProductFileType;				//	type of the product associated with this chunk
		YFlags						yfHeaderFlags;					//	attributes of this chunk's header
		YChunkId						yciNumberOfChunks;			//	number of chunks at this level
		YStreamPosition			ylChunkInfoPos;				//	offset of chunk info list from start of header
		YStreamLength				ylChunkAlignPad;				//	size of alignment padding in chunk's data
		YStreamPosition			ylChunkDataPos;				//	offset of data from start of header
		YStreamLength				ylChunkDataSize;				//	offset of data from start of header
		YVersion						yvFileSystemCurrentVersion;//	version of file format
		YVersion						yvFileSystemMinimumVersion;//	earliest file version compatible with this format
		YVersion						yvDataCurrentVersion;		//	version of file format
		YVersion						yvDataMinimumVersion;		//	earliest file version compatible with this format
		YChunkId						yIndexInAuxData;				//	index of this chunks aux data in aux data chunk storage
		
		//	Add new fields after yIndexInAuxData. Adjust the following padding area as needed.
		
		uBYTE							ubPadding[2];					//	unused
	};


// ****************************************************************************
//
// Class Name:			YChunkInfo
//
// Description:		holds location and content description of chunk data
//
// ****************************************************************************
//
class FrameworkLinkage YChunkInfo
{
	// Construction, Destruction & Initialization
	public :
										YChunkInfo( );
										~YChunkInfo( );
	public:
		YChunkTag					yChunkTag;						//	tag for this chunk
		YChunkId						yChunkId;						//	id for this chunk
		YFlags						yChunkFlags;					//	chunk attributes
		YStreamPosition			yChunkPos;						//	offset of chunk data from end of parent chunk's header
		YStreamLength				yChunkSize;						//	size of data for this chunk (including header, children, chunk infos and alignment padding)
		YPlatformTag				yPlatformTag;					//	native platform for this chunk
		RChunkStorage*				pChunk;							//	our in-memory storage
		uBYTE							ubPadding[4];					//	unused
};

class RCInfoList: public RArray<YChunkInfo> {};
class RChunkPtrList: public RArray<RChunkStorage*> {};

typedef	RChunkStorage*		YChunkStorageId;

// ****************************************************************************
//
// Class Name:			RChunkStorage
//
// Description:		Support for tagged block storage
//
// ****************************************************************************
//
class FrameworkLinkage RChunkStorage : public RStorage
	{
	public:
		//	iterator class
		class RChunkSearcher // : public RIterator
			{
				//	Member Data
				private :
					friend class RChunkStorage;
					RChunkStorage*			m_pTreeRoot;				//	the root of the chunk tree we are searching
					YChunkStorageId		m_pSavedSelection;		//	chunk selected when we started searching
					BOOLEAN					m_fRecursive;				//	TRUE: search the chunk tree at and below the current level, FALSE: just at the current level
					BOOLEAN					m_fUseTag;					//	TRUE: use the tag field when searching for chunks
					YChunkTag				m_yTag;						//	tag of chunk to search for
					BOOLEAN					m_fUseId;					//	TRUE: use the id field when searching for chunks
					YChunkId					m_yId;						//	id of chunk to search for
					BOOLEAN					m_fUseDataVersion;		//	TRUE: use the data version field when searching for chunks
					YVersion					m_yDataVersion;			//	data version of chunk to search for

				//	Construction & Destruction
				public :
											RChunkSearcher( ) 
												: m_pTreeRoot(NULL), m_pSavedSelection(NULL), m_fRecursive(FALSE), m_fUseTag(FALSE)
												, m_fUseId(FALSE), m_yId(0), m_yTag( kNoType ), m_fUseDataVersion( FALSE ), m_yDataVersion( kDontCareDataVersion )
												{
												NULL;
												}
											RChunkSearcher( const BOOLEAN fRecursive, const BOOLEAN fUseTag, const YChunkTag yTag, const BOOLEAN fUseId, const YChunkId yId, const BOOLEAN fUseDataVersion, const YVersion yDataVersion ) 
												: m_pTreeRoot(NULL), m_pSavedSelection(NULL), m_fRecursive(fRecursive), m_fUseTag(fUseTag), m_fUseId(fUseId), m_yId(yId), m_yTag( yTag ), m_fUseDataVersion( fUseDataVersion ), m_yDataVersion( yDataVersion )
												{
												NULL;
												}	
											RChunkSearcher( const RChunkSearcher& rhs ) 
												: m_pTreeRoot(rhs.m_pTreeRoot), m_pSavedSelection(rhs.m_pSavedSelection), m_fRecursive(rhs.m_fRecursive)
												, m_fUseTag(rhs.m_fUseTag), m_yTag( rhs.m_yTag ), m_yId( rhs.m_yId ), m_fUseId(rhs.m_fUseId)
												, m_fUseDataVersion(rhs.m_fUseDataVersion), m_yDataVersion(rhs.m_yDataVersion)
												{
												NULL;
												}
											~RChunkSearcher( )
												{
												NULL;
												}
					
					//	note that this member End() is usually in the container type's class but we need
					//	it in the iterator because different simultaneous searches can have different ends
					// depending on whether they are recursive or not
					BOOLEAN				End( ) const
											{
											BOOLEAN fEnd = static_cast<BOOLEAN>( NULL == m_pTreeRoot->GetSelectedChunk() );

											//	when were done restore the chunk tree's selection state so that another search will
											//	start in the same place this one did
											if ( fEnd )
												m_pTreeRoot->SetSelectedChunk( m_pSavedSelection );
											
											return fEnd;
											}

				//	Typedefs and Structs
//				public :
//					typedef	RChunkStorage	YContainerType;

				//	Operator overloads
				public :
					//	Asignment operator
					RChunkSearcher&	operator=( const RChunkSearcher& rhs )
											{
											m_pTreeRoot = rhs.m_pTreeRoot;
											m_pSavedSelection = rhs.m_pSavedSelection;
											m_fRecursive = rhs.m_fRecursive;
											m_fUseTag = rhs.m_fUseTag;
											m_yTag = rhs.m_yTag;
											m_fUseId = rhs.m_fUseId;
											m_yId = rhs.m_yId;
											m_fUseDataVersion = rhs.m_fUseDataVersion;
											m_yDataVersion = rhs.m_yDataVersion;
											return *this;
											}

					//	Dereference operators
					RChunkStorage		operator *( ) const
											{
											TpsAssert( m_pTreeRoot, "Data pointer is NULL" );
											return *m_pTreeRoot;
											}
					RChunkStorage&		operator *( )
											{
											TpsAssert( m_pTreeRoot, "Data pointer is NULL" );
											return *m_pTreeRoot;
											}
					RChunkStorage	operator []( int n ) const
											{
											if ( 0 != n )
												UnimplementedCode();
											TpsAssert( m_pTreeRoot, "Data pointer is NULL" );
											return *( m_pTreeRoot + n );
											}
					RChunkStorage&		operator []( int n )
											{
											if ( 0 != n )
												UnimplementedCode();
											TpsAssert( m_pTreeRoot, "Data pointer is NULL" );
											return *( m_pTreeRoot + n );
											}
					//	Increment / Decrement operator
					RChunkSearcher&	operator++( )			//	Prefix version
											{
											TpsAssert( m_pTreeRoot, "Data pointer is NULL" );
											
											//	Advance to next chunk with given id and tag. If we don't
											//	have an id to search for then just get the next chunk.
											BOOLEAN fEnd = FALSE;
											do 
												{
												if ( m_fUseTag )
													m_pTreeRoot->SelectNextChunk( m_pTreeRoot, m_yTag, m_fRecursive );
												else
													m_pTreeRoot->SelectNextChunk( m_pTreeRoot, m_fRecursive );
												fEnd = static_cast<BOOLEAN>( NULL == m_pTreeRoot->GetSelectedChunk() );
												}
											while( m_fUseId && !fEnd && (m_pTreeRoot->GetCurrentChunkId() != m_yId) );

											return *this;
											}
					RChunkSearcher		operator++( int )		//	Postfix version
											{
											TpsAssert( m_pTreeRoot, "Data pointer is NULL" );
											RChunkSearcher	iterator = *this;
											++*this;
											return iterator;
											}
					//	unimplemented
					//	Equality operators
					BOOLEAN			operator==(const RChunkSearcher& rhs) const
											{
											UnimplementedCode();
											return (BOOLEAN)(m_pTreeRoot == rhs.m_pTreeRoot);
											}
					BOOLEAN			operator!=(const RChunkSearcher& rhs) const
											{
											UnimplementedCode();
											return (BOOLEAN)(m_pTreeRoot != rhs.m_pTreeRoot);
											}
					BOOLEAN			operator<(const RChunkSearcher& rhs) const
											{
											UnimplementedCode();
											return (BOOLEAN)(m_pTreeRoot < rhs.m_pTreeRoot );
											}
					BOOLEAN			operator<=(const RChunkSearcher& rhs) const
											{
											UnimplementedCode();
											return (BOOLEAN)(m_pTreeRoot <= rhs.m_pTreeRoot );
											}
					BOOLEAN			operator>(const RChunkSearcher& rhs) const
											{
											UnimplementedCode();
											return (BOOLEAN)(m_pTreeRoot > rhs.m_pTreeRoot );
											}
					BOOLEAN			operator>=(const RChunkSearcher& rhs) const
											{
											UnimplementedCode();
											return (BOOLEAN)(m_pTreeRoot >= rhs.m_pTreeRoot );
											}
					//	unimplemented
					//	Increment / Decrement operator
					RChunkSearcher&	operator--( )			//	Prefix versino
											{
											TpsAssert( m_pTreeRoot, "Data pointer is NULL" );
											UnimplementedCode();
											--m_pTreeRoot;
											return *this;
											}
					RChunkSearcher		operator--( int )		//	Postfix version
											{
											TpsAssert( m_pTreeRoot, "Data pointer is NULL" );
											UnimplementedCode();
											RChunkSearcher	iterator = *this;
											--*this;
											return iterator;
											}
					//	unimplemented
					//	Addition / Subtraction operator
					RChunkSearcher&	operator+=( int n )
											{
											TpsAssert( m_pTreeRoot, "Data pointer is NULL" );
											UnimplementedCode();
											m_pTreeRoot	+= n;
											return *this;
											}
					RChunkSearcher&	operator-=( int n )
											{
											TpsAssert( m_pTreeRoot, "Data pointer is NULL" );
											UnimplementedCode();
											m_pTreeRoot	-= n;
											return *this;
											}
					RChunkSearcher	operator+( int n )
											{
											TpsAssert( m_pTreeRoot, "Data pointer is NULL" );
											UnimplementedCode();
											RChunkSearcher iter( *this );
											iter += n;
											return iter;
											}
					RChunkSearcher	operator-( int n )
											{
											TpsAssert( m_pTreeRoot, "Data pointer is NULL" );
											UnimplementedCode();
											RChunkSearcher iter( *this );
											iter -= n;
											return iter;
											}
			} ;
		//	Define a search parameter class to overcome difficulties with ambiguous start parameter lists.
			class RChunkSearchParams  : public RChunkSearcher
				{								
				public:	
					RChunkSearchParams( const BOOLEAN fRecursive, const BOOLEAN fUseTag, const YChunkTag yTag, const BOOLEAN fUseId, const YChunkId yId, const BOOLEAN fUseDataVersion, const YVersion yDataVersion ) 
					: RChunkSearcher( fRecursive, fUseTag, yTag, fUseId, yId, fUseDataVersion, yDataVersion )
					{
					NULL;
					}
				};

	// Construction, Destruction & Initialization
	public :
										RChunkStorage( );
										RChunkStorage( YStreamLength ySize, PUBYTE pubSource );
										RChunkStorage( const RMBCString& rFilename, EAccessMode eOpenMode, BOOLEAN fPlaceholder = FALSE );
										RChunkStorage( RStorage* pParent, BOOLEAN fPlaceholder = FALSE );
		virtual void				CreateFromStorage( RStorage* pParent, BOOLEAN fPlaceholder = FALSE );
										RChunkStorage( BOOLEAN fMemoryBackStore );
										RChunkStorage( RStorage* pParent, const YStreamPosition yHeaderPosition, const YStreamLength yChunkSize );
		virtual void				CreateSubtreeFromStorage( const BOOLEAN fPlaceholder = FALSE );
		virtual						~RChunkStorage( );

	// Operations
	public :
		virtual void				Read( YStreamLength yBufferSize, uBYTE* pubBuffer );
		virtual void				Write( YStreamLength yBufferSize, const uBYTE* pubBuffer );
		virtual void				SeekAbsolute( const YStreamLength ySeekPosition );
		virtual void				SeekRelative( const YStreamOffset ySeekOffset );
		virtual YStreamLength	GetPosition( ) const;
		virtual YStreamLength	GetAbsolutePositionInStream( ) const;
		static YStreamLength		GetHeaderSizeInStream( RStorage* pParent );

	// Chunk Specific classes
	public :
		YChunkId						AddSubChunk( const YChunkTag yChunkTag );
		virtual YChunkId			AddChunk( const YChunkTag yChunkTag );
		YChunkStorageId			GetSelectedChunk( ) const;
		void							SetSelectedChunk( YChunkStorageId chunkId );
		void							SelectParentChunk( );
		void							SelectRootChunk( );
		void							Commit( BOOLEAN fFlush = FALSE );
		void							MarkSelectedChunkDeleted( );
		void							StorageDirty( );
		BOOLEAN						StreamExists( ) const;
		YVersion						GetRenaissanceFileSystemVersion();
		YVersion&					GetCurrentChunkCurrentFileSystemVersion();
		YVersion&					GetCurrentChunkMinFileSystemVersion();
		YVersion&					GetCurrentChunkCurrentDataVersion();
		YVersion&					GetCurrentChunkMinDataVersion();
		static BOOLEAN				IsChunkStorage( RStorage* pStorage );

		virtual YAuxSize			GetAuxDataSize();

		YChunkId						GetCurrentChunkId( ) const;
		YChunkTag					GetCurrentChunkTag( ) const;
		YStreamLength				GetCurrentChunkSize( ) const;
		YStreamLength				GetCurrentChunkDataSize( ) const;
		BOOLEAN						IsCurrentChunkNested( ) const;
		BOOLEAN						IsCurrentChunkCurrentPlatform( ) const;

		void							ReadAuxData( const YStreamLength yBufferSize, uBYTE* pubBuffer );
		void							WriteAuxData( const YStreamLength yBufferSize, const uBYTE* pubBuffer );

		YChunkId						GetCurrentNumberOfChunks( const BOOLEAN fRecursive = FALSE );
		YChunkId						GetCurrentNumberOfChunks( const YChunkTag yChunkTag, const BOOLEAN fRecursive = FALSE );
		void							SelectFirstChunk( );
		void							SelectFirstChunk( RChunkStorage* pSearchRoot, const YChunkTag yTagType, const BOOLEAN fRecursive = FALSE );
		void							SelectNextChunk( RChunkStorage* pSearchRoot, const YChunkTag yTagType, const BOOLEAN fRecursive = FALSE );
		void							SelectNextChunk( RChunkStorage* pSearchRoot, const BOOLEAN fRecursive = FALSE );
		
		RChunkSearcher				Start( const BOOLEAN fRecursive = FALSE );
		RChunkSearcher				Start( const YChunkTag yTagType, const BOOLEAN fRecursive = FALSE );
		RChunkSearcher				Start( const YChunkTag yChunkTag, const YChunkId yChunkId, const BOOLEAN fRecursive /*= FALSE*/ );
		RChunkSearcher				Start( RChunkSearcher rSearcher );

		void							SetSaveOnClose( const BOOLEAN fSave = TRUE );
		static RChunkBackstore*	GetRootBackstore( const BOOLEAN fCreate = kCreateBackstoreIfNecessary );
		static void					DeleteBackstore( );
		void							Save( RChunkStorage& rDest );
		PUBYTE						GetCurrentChunkBuffer( YStreamLength& yBufferSize ) const;
		PUBYTE						GetCurrentChunkDataBuffer( YStreamLength& yBufferSize ) const;
		void							ReleaseBuffer( PUBYTE pubBuffer ) const;
		void							WriteFailed( const BOOLEAN fDeleteOnClose );
		virtual void				PreserveFileDate( const BOOLEAN fPreserveFileDate = TRUE );
		virtual BOOLEAN			ShouldPreserveFileDate( );
		
		// exposed to be able to access base class functions such as RStorage::SeekAbsolute
		RStorage*					GetDataParent( ) const;

#ifdef TPSDEBUG
	// Debugging Support
	public :
		virtual void				Validate( ) const;

		//	function for testing only:
		RChunkBackstore*			GetAuxDataMember();
#endif

	// Implementation
	protected :
		YChunkStorageId			GetFirstChunkChild( );
		YChunkStorageId			GetCurrentNextChunk( const BOOLEAN fRecursive = FALSE );
		virtual void				ReadChunkIndex( const BOOLEAN fForceUpdate );
		RChunkBackstore*			GetRootAuxData( );
		RChunkStorage*				GetAuxData( );
		void							GetChunkListStart( RCInfoList::YIterator& rIter );
		YCounter						GetChunkListCount( );
		YChunkStorageId			GetFirstChunk( );
		YChunkStorageId			GetFirstChunk( YStreamPosition& yPos );
		void							GetStorageAndPos( YStreamPosition& yPos, RStorage*& pStorage );
		static RStorage*			GetStorage( RStorage* pStorage );
		virtual void				ReadInternal( const YStreamLength yBufferSize, uBYTE* pubBuffer );
		virtual void				WriteInternal( const YStreamLength yBufferSize, const uBYTE* pubBuffer );
		virtual void				SeekAbsoluteInternal( const YStreamLength ySeekPosition );
		virtual void				SeekRelativeInternal( const YStreamOffset ySeekOffset );
		virtual YStreamLength	GetPositionInternal( ) const;
		void							WriteIndex( );
		YChunkId						GetNumberOfChunks( const BOOLEAN fRecursive = FALSE );
		YChunkId						GetNumberOfChunks( const YChunkTag yTagType, const BOOLEAN fRecursive = FALSE );
		YChunkStorageId			GetNextChunk( RChunkStorage* pSearchRoot, const BOOLEAN fRecursive = FALSE );
		YChunkStorageId			GetNextChunk( RChunkStorage* pSearchRoot, const YChunkTag yChunkTag, const BOOLEAN fRecursive = FALSE );
		virtual YStreamLength	GetDataSize() const;
		YChunkId						GetDataParentIndex( ) const;
		void							Copy( const YChunkInfo& yci, YStreamLength yNewSize );
		virtual YStreamLength	GetSize() const;
		BOOLEAN						IsInBackstore();

	// Implementation
	private :
		void							DoneWriting( );
		virtual void				Grow( const YStreamLength yNewSize );
		void							UpdateParentSize( BOOLEAN fSetMaxSize = FALSE );
		void							WriteHeader( BOOLEAN fFirstWrite = FALSE );
		void							DeleteMembers( );
		YChunkInfo&					GetIndex( ) const;
		BOOLEAN						IsRoot( ) const;
		BOOLEAN						HasAuxData() const;
		void							ReadHeader( RStorage* pStorage );

	// Member data
	private :
		YFlags						m_yRootFlags;				//	chunk storage attributes
		YChunkStorageId			m_pSelectedStorage;		//	chunk that api functions apply to
		RStorage* 					m_pParentStorage;			//	storage for chunks
		RStorage* 					m_pDataParentStorage;	//	storage for chunks data when they are in the backstore
		YChunkHeader				m_ChunkHeader;				//	description of chunk data
		RChunkBackstore*			m_pAuxData;					//	aux data when in memory
		static RChunkBackstore*	m_pBackstore;				//	global backstore for all chunk storages
		RCInfoList					m_ChunkInfoList;			//	list of indices for chunks
		YChunkId						m_yIndexInParent;			//	index of this chunk in our parents index
		YChunkId						m_yIndexInDataParent;	//	index of this chunk in our parents index in the backstore
		YStreamPosition			m_yMarker;					//	current file pointer relative to start of this chunk
		YStreamLength				m_ySize;						//	currently used size of chunk not including chunk's header or indices
		YStreamLength				m_yMaxSize;					//	largest size of chunk before it must be written to backstore not including chunk's header
		RStorage*					m_prExistingStorage;		//	existing storage on disk when file is opened for safe save
	};



// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_CHUNKY_STORAGE_H_
