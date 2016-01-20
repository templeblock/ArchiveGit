// ****************************************************************************
//
//  File Name:			ChunkStorage.cpp
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
//  $Logfile:: /PM8/Framework/Source/ChunkyStorage.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************
#include "FrameworkIncludes.h"

ASSERTNAME

#include "ChunkyStorage.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"ChunkyBackstore.h"

#ifndef kFILETESTPROJECT
	#include		"ApplicationGlobals.h"
#endif

#include	"ChunkyBackstore.h"
#include "FileStream.h"
#include "MemoryStream.h"
#include "BufferStream.h"

//	minimum size of chunks in backstore; the larger this is the 
//	less often the chunk will need to grow
const uLONG kChunkMinSize = 50;

//	Chunk storage file system version info:
//	INCREMENT BOTH kCurrentRenaissanceFileSystemVersion AND kMinRenaissanceFileSystemVersion
//	WHEN THE CHUNK STORAGE FORMAT CHANGES.
const YVersion kCurrentRenaissanceFileSystemVersion =	13;
const YVersion kMinRenaissanceFileSystemVersion =	13;

//	This variable used in ReadChunkIndex if TRUE forces a chunk's indices to be loaded all at once. If FALSE chunk's
//	indices are demand loaded.
const BOOLEAN kLoadAllChunkIndicesRecursively = FALSE;

//	initialize global backstore ptr
RChunkBackstore*	RChunkStorage::m_pBackstore = NULL;

//#define kFORCEFIXBACKSTORECHUNK			//	copy size of chunk into backstore chunk to fix backstore size update problem
//#define kSWITCHINDEXONCHUNKPARENTTYPE	//	try to fix index acqusition problem by swapping backstore and chunk's parent indices
//#define kCLEARCOPIEDINDICES				//	make moved indices write again after they are copied
#define kPOSTCOPYWRITEFIX						//	copy size of written data to moved backstore chunk
// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetSelectedChunk( )
//
//  Description:		return currently selected chunk 
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YChunkStorageId RChunkStorage::GetSelectedChunk( ) const
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );

	return m_pSelectedStorage;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::SelectedRootChunk( )
//
//  Description:		set currently selected chunk to parent
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChunkStorage::SelectRootChunk( )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );

	RChunkStorage *pRoot = GetFirstChunk();
	pRoot->SetSelectedChunk( pRoot );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::SetSelectedChunk( )
//
//  Description:		set currently selected chunk 
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChunkStorage::SetSelectedChunk( YChunkStorageId pChunk )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );

	m_pSelectedStorage = pChunk;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::IsRoot( )
//
//  Description:		return true if our parent is an RStorage 
//								(so we are the root of the chunk tree).
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RChunkStorage::IsRoot( ) const
	{
// REVIEW GCB 10/10/96 fix compile error on the Mac
// 1) CodeWarrior gives "constness casted away" error
// 2) if dynamic_cast fails it returns NULL - is that accounted for here?!
#if defined MAC
	return static_cast<BOOLEAN>(NULL == dynamic_cast<RChunkStorage*>(const_cast<RStorage*>(m_pParentStorage)));
#else
	return static_cast<BOOLEAN>(NULL == dynamic_cast<RChunkStorage*>(m_pParentStorage));
#endif
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::SetSaveOnClose( const BOOLEAN fSave = TRUE )
//
//  Description:		if fSave is TRUE: safe save the chunk tree when it is destructed 
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChunkStorage::SetSaveOnClose( const BOOLEAN fSave /*= TRUE*/ )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	
	if ( fSave )
		SetFlag( m_yRootFlags, kReplaceExisting ); 
	else
		ClearFlag( m_yRootFlags, kReplaceExisting );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetAuxDataMember( )
//
//  Description:		return a ptr to our aux data 
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
#ifdef TPSDEBUG
RChunkBackstore* RChunkStorage::GetAuxDataMember( )
	{
	return m_pAuxData;
	}
#endif


// ****************************************************************************
//
//  Function Name:	RStorage::WriteFailed( )
//
//  Description:		We failed while writing. Remember not to write again.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChunkStorage::WriteFailed( const BOOLEAN fDeleteOnClose )
	{
	RStorage* pStorageRoot = GetStorage( this );
	if( pStorageRoot )
		pStorageRoot->WriteFailed( fDeleteOnClose );
	}

// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetRootAuxData( )
//
//  Description:		return the aux data for this tree of chunks.
//
//  Returns:			the aux data for this tree of chunks;
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChunkBackstore* RChunkStorage::GetRootAuxData( )
	{
	RChunkBackstore* pAuxData = m_pAuxData;
	RChunkStorage* pChunk;

	//	seek the root of this chunk tree and return its aux data
	for (pChunk = dynamic_cast<RChunkStorage*>(m_pParentStorage); 
		(NULL != pChunk) && (NULL == pChunk->GetStorageStream()); 
		pChunk = dynamic_cast<RChunkStorage*>(pChunk->m_pParentStorage))
		pAuxData = pChunk->m_pAuxData;

	return pAuxData;
	}

// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetRootBackstore( const BOOLEAN fCreate = kCreateBackstoreIfNecessary )
//
//  Description:		return the backstore for all trees of chunks.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChunkBackstore* RChunkStorage::GetRootBackstore( const BOOLEAN fCreate /*= kCreateBackstoreIfNecessary*/ )
	{
	//	We only support the backstore on the mac. Windows VM obviates the need for it.

	//	REVIEW ESV 1/29/97 - Implement this when the text and headline components have been modified to not need the backstore.
//	WinCode( return NULL );

	if ( fCreate && (NULL == m_pBackstore) )
		m_pBackstore = new RChunkBackstore;

	return m_pBackstore;
//	return GetRootAuxData();	//	temporarily use aux data so we can see its contents after the storage closes
	}


// ****************************************************************************
//
//  Function Name:	YChunkInfo::YChunkInfo( )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
YChunkInfo::YChunkInfo( )
	: yChunkTag(0)
	, yChunkId(kNoFlags)
	, yChunkFlags(0)
	, yChunkPos(kInvalidChunkPos)
	, yChunkSize(0)
	, pChunk(NULL)
#ifdef _WINDOWS
	, yPlatformTag(kWin)
#elif defined MAC
	, yPlatformTag(kMac)
#endif
	{
	ubPadding[0] = 0;
	ubPadding[1] = 0;
	ubPadding[2] = 0;
	ubPadding[3] = 0;
	}


// ****************************************************************************
//
//  Function Name:	YChunkInfo::YChunkInfo( )
//
//  Description:		dtor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
YChunkInfo::~YChunkInfo( )
	{
	NULL;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::YChunkInfo( )
//
//  Description:		return the size of the aux data associated with 
//							the currently selected chunk.
//
//  Returns:			^
//
//  Exceptions:		file
//
// ****************************************************************************
//
YAuxSize RChunkStorage::GetAuxDataSize( )
	{
	YAuxSize ySize = 0;
	
	if ( HasAuxData() )
		ySize = GetAuxData( )->GetCurrentChunkDataSize();

	return ySize;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::HasAuxData( )
//
//  Description:		return true if aux data is associated with this chunk.
//
//  Returns:			^
//
//  Exceptions:		none
//
// ****************************************************************************
//
BOOLEAN RChunkStorage::HasAuxData( ) const
	{
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
	TpsAssert( IsRoot(), "illegal access of internal tree node" );

	return static_cast<BOOLEAN>(kInvalidChunkIndex != m_pSelectedStorage->m_ChunkHeader.yIndexInAuxData);
	}


// ****************************************************************************
//
//  Function Name:	YChunkHeader::YChunkHeader( )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
YChunkHeader::YChunkHeader( )
	: ulMagicKey( kMagicKey )
#ifdef _WINDOWS
	, yPlatformTag(kWin)
#elif defined MAC
	, yPlatformTag(kMac)
#endif
	, yHeaderSize(sizeof(YChunkHeader))
	, yProductTag(kRenaissance)
	, yProductFileType(kRenaissanceDoc)
	, yfHeaderFlags(kNoFlags)
	, yciNumberOfChunks(0)
	, ylChunkInfoPos(kInvalidChunkPos)
	, ylChunkAlignPad(0)
	, ylChunkDataPos(kInvalidChunkPos)
	, ylChunkDataSize(0)
	, yIndexInAuxData(kInvalidChunkIndex)
	, yvFileSystemCurrentVersion(kCurrentRenaissanceFileSystemVersion)
	, yvFileSystemMinimumVersion(kMinRenaissanceFileSystemVersion)
	, yvDataCurrentVersion(kDefaultDataVersion)
	, yvDataMinimumVersion(kDefaultDataVersion)
	{
	ubPadding[0] = 0;
	ubPadding[1] = 0;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::RChunkStorage( )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChunkStorage::RChunkStorage( )
	: RStorage( )
	, m_pParentStorage(NULL)
	, m_pSelectedStorage(NULL)
	, m_yRootFlags( kWriteClosingIndex | kWriteAuxDataOnClose )
	, m_yIndexInParent(kInvalidChunkIndex)
	, m_yMarker(0)
	, m_ySize(0)
	, m_yMaxSize(kMaxChunkSize)
	, m_pAuxData(NULL)
	, m_prExistingStorage(NULL)
	, m_yIndexInDataParent(kInvalidChunkIndex)
	, m_pDataParentStorage(NULL)
	{
	NULL;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::RChunkStorage( YStreamLength ySize, PUBYTE pubSource )
//
//  Description:		Constructor that copies given memory area into the chunk storage.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChunkStorage::RChunkStorage( YStreamLength ySize, PUBYTE pubSource )
	: RStorage( )
	, m_pParentStorage(NULL)
	, m_yRootFlags( kWriteClosingIndex | kDeleteStorageOnClose | kWriteAuxDataOnClose )
	, m_yIndexInParent(kInvalidChunkIndex)
	, m_yMarker(0)
	, m_ySize(0)
	, m_yMaxSize(kMaxChunkSize)
	, m_pAuxData(NULL)
	, m_prExistingStorage(NULL)
	, m_yIndexInDataParent(kInvalidChunkIndex)
	, m_pDataParentStorage(NULL)
	{
	RBufferStream *pStream = NULL;

	try
		{
		m_pSelectedStorage = this;

		try
			{
			pStream				= new RBufferStream( pubSource, ySize );
			m_pParentStorage	= new RStorage( pStream );
			}
		catch(...)
			{
			delete pStream;
			throw;
			}

		m_pParentStorage->DeleteStreamOnClose();
		SetFlag( m_yRootFlags, kDeleteStorageOnClose );
		
		if ( NULL != pStream )
			{
			YStreamLength yLength = pStream->GetSize();

			if ( 0 == yLength )	//	if were creating a new archive add the header for this chunk
				{
				WriteHeader( kFirstWrite );	//	mark writing of the storage as incomplete
				}
			else
				{
				//	remember to not write the index for this header since it already exists
				//	(were opening an existing file)
				ClearFlag( m_yRootFlags, kWriteClosingIndex );
				SetFlag( m_yRootFlags, kOpenedExisting );
				ReadChunkIndex( !kForceUpdate );
				}
			}
		}
	catch(...)
		{
		DeleteMembers();
		throw;
		}
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::RChunkStorage( BOOLEAN fMemoryBackStore )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChunkStorage::RChunkStorage( BOOLEAN fMemoryBackStore )
	: RStorage( )
	, m_pParentStorage(NULL)
	, m_yRootFlags( kWriteClosingIndex | kDeleteStorageOnClose | kWriteAuxDataOnClose )
	, m_yIndexInParent(kInvalidChunkIndex)
	, m_yMarker(0)
	, m_ySize(0)
	, m_yMaxSize(kMaxChunkSize)
	, m_pAuxData(NULL)
	, m_prExistingStorage(NULL)
	, m_yIndexInDataParent(kInvalidChunkIndex)
	, m_pDataParentStorage(NULL)
	{
	try
		{
		m_pSelectedStorage = this;

#ifdef MAC // GCB 10/18/96 RMemoryStream not implemented on the Mac
		fMemoryBackStore = FALSE;
#endif

		if ( fMemoryBackStore )
			{
#if ! defined MAC // GCB 10/18/96 RMemoryStream not implemented on the Mac
			RMemoryStream *prmStream = NULL;

			try
				{
				//	allocate a memory stream to back the backstore
				prmStream = new RMemoryStream;
				prmStream->Open( kReadWrite );
				m_pParentStorage = new RStorage( prmStream );
				}
			catch(...)
				{
				delete prmStream;
				throw;
				}
#endif
			}
		else
			{
			//	if we fail to create the parent storage make sure we delete the stream backing it
			//	allocate a temp file to back the backstore 
			RFileStream *prfStream = NULL;

			try
				{
				prfStream = new RFileStream;
				prfStream->CreateTempFile();
				prfStream->Open( kReadWrite );
				m_pParentStorage = new RStorage( prfStream );
				}
			catch(...)
				{
				//	if we catch an exception opening a read/write file
				//	we need to delete the file on close
				if( NULL != prfStream )
					prfStream->DeleteOnClose( TRUE );
				delete prfStream;
				throw;
				}
			}

		m_pParentStorage->DeleteStreamOnClose();
		SetFlag( m_yRootFlags, kDeleteStorageOnClose );
		
		//	get stream storage for all chunks in this tree of chunks
		RStream* pStream = m_pParentStorage->GetStorageStream();	
		if ( NULL != pStream )
			{
			YStreamLength yLength = pStream->GetSize();

			if ( 0 == yLength )	//	if were creating a new archive add the header for this chunk
				{
				WriteHeader( kFirstWrite );	//	mark writing of the storage as incomplete
				}
			else
				{
				//	remember to not write the index for this header since it already exists
				//	(were opening an existing file)
				ClearFlag( m_yRootFlags, kWriteClosingIndex );
				SetFlag( m_yRootFlags, kOpenedExisting );
				ReadChunkIndex( !kForceUpdate );
				}
			}
		}
	catch(...)
		{
		DeleteMembers();
		throw;
		}
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::RChunkStorage( RStorage* pStorage, BOOLEAN fPlaceholder )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		file
//
// ****************************************************************************
//
RChunkStorage::RChunkStorage( RStorage* pStorage, const YStreamPosition yHeaderPos, const YStreamLength yChunkSize )
	: RStorage( )
	, m_pParentStorage(pStorage)
	, m_yRootFlags( kNoRootFlags )
	, m_yIndexInParent(kInvalidChunkIndex)
	, m_yMarker(0)
	, m_ySize(0)
	, m_yMaxSize(kMaxChunkSize)
	, m_pAuxData(NULL)
	, m_prExistingStorage(NULL)
	, m_yIndexInDataParent(kInvalidChunkIndex)
	, m_pDataParentStorage(NULL)
	{
	//	Load a chunk tree from the given location in the given storage.

	//	Put the new tree under this (root) chunk.
	//	Install a tag value that indicates that it is a subtee created 
	//	without context from the given location int the given storage.
	YChunkInfo cInfo;
	cInfo.yChunkTag = kSubTreeRootTag;
	cInfo.yChunkPos = yHeaderPos;
	cInfo.yChunkSize = yChunkSize;
	m_ySize = yChunkSize;	
	// cInfo.yChunkId = Use the default.

	//	Make a new chunk storage to hold the given stream contents.
	//	Remember that we allocated this chunk and should delete it.
	//	make the chunk this index refers to and remember it for later.
	RChunkStorage* pChild = new RChunkStorage();
	SetFlag( cInfo.yChunkFlags, kDeleteChunkPtrOnClose);
	cInfo.pChunk = pChild;										
	
	//	Read and validate the header.
	pStorage->SeekAbsolute( cInfo.yChunkPos );
	pChild->ReadHeader( pStorage );
	
	//	Adjust the chunk pos to where it would be if chunk
	//	we have added to the tree in memory were also on disk.
	//	We will never try to read that header so it is ok that the 
	//	header isn't really on disk.
	cInfo.yChunkPos -= pChild->m_ChunkHeader.yHeaderSize;
	
	//	Load the chunk tree under this root.
	m_ChunkHeader.ylChunkInfoPos = 0;
	m_ChunkHeader.yciNumberOfChunks = 1;
	m_ChunkInfoList.InsertAtEnd( cInfo );

	//	Link the sub tree in the root's indices.
	//	the file was opened for read access; don't write anything
	SetFlag( m_yRootFlags, kIndexWritten );	
	ClearFlag( m_yRootFlags, kWriteClosingIndex );
	SetFlag( cInfo.pChunk->m_yRootFlags, kIndexWritten );	
	cInfo.pChunk->m_pParentStorage = this;
	cInfo.pChunk->m_yIndexInParent = m_ChunkInfoList.Count()-1;
	ClearFlag( cInfo.pChunk->m_yRootFlags, kWriteClosingIndex );

	//	Select the new subtree.
	m_pSelectedStorage = pChild;
	}

// ****************************************************************************
//
//  Function Name:	RChunkStorage::CreateSubtreeFromStorage( RStorage* pStorage, BOOLEAN fPlaceholder )
//
//  Description:		Create a chunk storage tree under the current selected chunk.
//
//  Returns:			Nothing;
//
//  Exceptions:		file
//
// ****************************************************************************
//
void RChunkStorage::CreateSubtreeFromStorage( const BOOLEAN fPlaceholder /*= FALSE*/ )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
	
	//	make sure buffer contains updated information for chunk lists, etc
	m_pSelectedStorage->CreateFromStorage( this, fPlaceholder );

	//	Select the root of our local tree so that searches will find all their data
	//	in the subtree.
	m_pSelectedStorage = this;
	SelectFirstChunk( );
	}


// ****************************************************************************
//
// Function Name:		RChunkStorage::GetCurrentChunkId( )
//
// Description:		return the index of the current chunk
//
// Returns:				^
//
// Exceptions:			memory
//
// ****************************************************************************
//
YChunkId RChunkStorage::GetCurrentChunkId( ) const
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
	return m_pSelectedStorage->GetIndex().yChunkId;
	}


// ****************************************************************************
//
// Function Name:		RChunkStorage::GetSize( )
//
// Description:		return the size of the chunk
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
YStreamLength RChunkStorage::GetSize( ) const
	{
	return m_ySize;
	}


// ****************************************************************************
//
// Function Name:		RChunkStorage::GetDataSize( )
//
// Description:		return the size of the selected chunk's data
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
YStreamLength RChunkStorage::GetDataSize( ) const
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	return m_pSelectedStorage->m_ChunkHeader.ylChunkDataSize;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::RChunkStorage( const RMBCString& rFilename, EAccessMode eOpenMode )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		file
//
// ****************************************************************************
//
RChunkStorage::RChunkStorage( const RMBCString& rFilename, EAccessMode eOpenMode, BOOLEAN fPlaceholder /*= FALSE*/ )
	: RStorage( )
	, m_pParentStorage(NULL)
	, m_yRootFlags( kWriteClosingIndex | kWriteAuxDataOnClose )
	, m_yIndexInParent(kInvalidChunkIndex)
	, m_yMarker(0)
	, m_ySize(0)
	, m_yMaxSize(kMaxChunkSize)
	, m_pAuxData(NULL)
	, m_prExistingStorage(NULL)
	, m_yIndexInDataParent(kInvalidChunkIndex)
	, m_pDataParentStorage(NULL)
	{
	RStorage* pStorage = NULL;

	try
		{
		pStorage = new RStorage( rFilename, eOpenMode );
		CreateFromStorage( pStorage, fPlaceholder );

		//	Remember that we need to delete the storage newed above.
		//	Only set this flag after we know that the chunk storage is ready to
		//	destruct and clean up pStorage.
		SetFlag( m_yRootFlags, kDeleteStorageOnClose );
		}
	catch( ... )
		{
		delete pStorage;
		throw;
		}
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::ReadHeader( RStorage* pStorage )
//
//  Description:		Read and validate our header.
//
//  Returns:			Nothing;
//
//  Exceptions:		file
//
// ****************************************************************************
//
void RChunkStorage::ReadHeader( RStorage* pStorage )
	{
	TpsAssert( pStorage, "Null input storage" );

	//	pStorage must be positioned at the top of the header. Remember where we are for future reference.
	YStreamPosition yHeaderPos = pStorage->GetPosition();

	//	Pick up the disk header's size. Seek past prior header fields.
	pStorage->Read( sizeof( m_ChunkHeader.ulMagicKey ) + sizeof( m_ChunkHeader.yPlatformTag ) + sizeof( m_ChunkHeader.yHeaderSize )
		, (PUBYTE)&(m_ChunkHeader) );
	
	//	Handle byte swapping in header size.
	if (m_ChunkHeader.yPlatformTag != RStorage::GetPlatformTag( ))
		m_ChunkHeader.yHeaderSize = tintSwapWord( m_ChunkHeader.yHeaderSize );

	//	Seek back to the beginning of the header so we may load it in it's entirety now that we know it's size.
	pStorage->SeekAbsolute( yHeaderPos );
	
	//	Load the current header. If we are loading a header smaller (from an older version) than the one we are using now
	//	use its size and leave the remaining fields of the header initialized with default values.
	pStorage->Read( Min( (uWORD)sizeof( m_ChunkHeader ), m_ChunkHeader.yHeaderSize ), (PUBYTE)&(m_ChunkHeader) );
	
	// GCB 10/21/96 - handle byte swapping
	if (m_ChunkHeader.yPlatformTag != RStorage::GetPlatformTag( ))
		{
#ifdef _WINDOWS
		TpsAssert( m_ChunkHeader.yPlatformTag == kMac, "reading invalid data or a chunk written for an unsupported platform" );
#elif defined MAC
		TpsAssert( m_ChunkHeader.yPlatformTag == kWin, "reading invalid data or a chunk written for an unsupported platform" );
#endif
		m_ChunkHeader.ulMagicKey = tintSwapLong( m_ChunkHeader.ulMagicKey );
		m_ChunkHeader.yHeaderSize = tintSwapWord( m_ChunkHeader.yHeaderSize );
		m_ChunkHeader.yProductTag = (YProductTag)tintSwapLong( (uLONG)m_ChunkHeader.yProductTag );
		m_ChunkHeader.yProductFileType = (YChunkType)tintSwapLong( (uLONG)m_ChunkHeader.yProductFileType );
		m_ChunkHeader.yfHeaderFlags = tintSwapLong( m_ChunkHeader.yfHeaderFlags );
		m_ChunkHeader.yciNumberOfChunks = tintSwapLong( m_ChunkHeader.yciNumberOfChunks );
		m_ChunkHeader.ylChunkInfoPos = tintSwapLong( m_ChunkHeader.ylChunkInfoPos );
		m_ChunkHeader.ylChunkAlignPad = tintSwapLong( m_ChunkHeader.ylChunkAlignPad );
		m_ChunkHeader.ylChunkDataPos = tintSwapLong( m_ChunkHeader.ylChunkDataPos );
		m_ChunkHeader.ylChunkDataSize = tintSwapLong( m_ChunkHeader.ylChunkDataSize );
		m_ChunkHeader.yIndexInAuxData = tintSwapLong( m_ChunkHeader.yIndexInAuxData );
		}

	//	verify that our data is valid; throw an exception if it isn't
	TpsAssert( m_ChunkHeader.ulMagicKey == kMagicKey, "reading invalid data or a chunk written for another version of renaissance" );
	Debug( ThrowFileExceptionIfTesting( kDataFormatInvalid ) );
	
	if ( m_ChunkHeader.ulMagicKey != kMagicKey ) 
		ThrowFileException( kDataFormatInvalid );

	//	use my header to calculate the size of my data (excluding my header and indices)
	m_ySize = m_ChunkHeader.ylChunkInfoPos - m_ChunkHeader.ylChunkAlignPad;	

	//	we cannot grow without special action in a completely constructed chunk	
	m_yMaxSize = m_ySize;		
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::RChunkStorage( RStorage* pStorage, BOOLEAN fPlaceholder )
//
//  Description:		ctor.
//
//  Returns:			Nothing;
//
//  Exceptions:		file
//
// ****************************************************************************
//
RChunkStorage::RChunkStorage( RStorage* pStorage, BOOLEAN fPlaceholder /*= FALSE*/ )
	: RStorage( )
	, m_pParentStorage(NULL)
	, m_yRootFlags( kWriteClosingIndex | kWriteAuxDataOnClose )
	, m_yIndexInParent(kInvalidChunkIndex)
	, m_yMarker(0)
	, m_ySize(0)
	, m_yMaxSize(kMaxChunkSize)
	, m_pAuxData(NULL)
	, m_prExistingStorage(NULL)
	, m_yIndexInDataParent(kInvalidChunkIndex)
	, m_pDataParentStorage(NULL)
	{
	CreateFromStorage( pStorage, fPlaceholder );
	}

// ****************************************************************************
//
//  Function Name:	RChunkStorage::CreateFromStorage( RStorage* pStorage, BOOLEAN fPlaceholder )
//
//  Description:		common ctor code.
//
//  Returns:			Nothing;
//
//  Exceptions:		file
//
// ****************************************************************************
//
void RChunkStorage::CreateFromStorage( RStorage* pStorage, BOOLEAN fPlaceholder /*= FALSE*/ )
	{
	try
		{
		//	If were given a chunk storage then use the storage backing it.
		RStorage* pStorageToCreateFrom = RChunkStorage::GetStorage( pStorage );
		
		//	Get the stream backing the storage.
		RStream* pStorageStream = pStorageToCreateFrom->GetStorageStream();

		EAccessMode eOpenMode = pStorageStream->GetAccessMode();

		//	If we are opening for reading, make sure it is a chunk storage before initialization.
		// We dont care if we are writing.
		if ( ( kRead == eOpenMode || kReadWrite == eOpenMode ) && !IsChunkStorage( pStorage ) )
			ThrowFileException( kDataFormatInvalid );

		m_pSelectedStorage = this;
		
		//	Get a new aux data. Don't bother for read only streams since 
		//	we won't be able to change it anyway.
		if ( pStorageStream && (kRead != pStorageStream->GetAccessMode()) && (NULL == GetRootAuxData()) )
			m_pAuxData = new RChunkBackstore( kMemoryStream );

		//	We operate in one of three modes:
		//	1. we are operating in a new stream or an existing stream we will not change (it is read only).
		//	2. we are safe saving over an existing stream so work in a temp file.
		//	3. we are operating in a an existing stream we will change (it is read-write).
		//	If kRead is our access mode we're opening the stream to read and we are using mode 1.
		//	If the stream already has data we need to protect it so we operate in mode 2.
		//	If the stream has no data we operate in mode 1.
		if ( fPlaceholder || (kRead == eOpenMode) || (0 == pStorageStream->GetSize( )) )
			{
			//	Operate in mode 1. Use the existing stream to back our operations.
			m_pParentStorage = pStorage;		//	use existing file
			
			//	We may have an existing index. If so don't write a new one.
			if ( fPlaceholder || (kRead == eOpenMode) )
				{
				//	Remember to not write the index or aux data for this chunk since it already exists
				//	since were opening an existing file
				//	or if its a placeholder since we dont care.
				SetFlag( m_yRootFlags, kIndexWritten );
				ClearFlag( m_yRootFlags, kWriteClosingIndex );
				ClearFlag( m_yRootFlags, kWriteAuxDataOnClose );
				}

			//	If we are opening an existing stream which has data read the index
			//	else prep the empty stream for use by writing the initial header.
			if ( 0 < pStorageStream->GetSize( ) )
				{
				SetFlag( m_yRootFlags, kOpenedExisting );
				ReadChunkIndex( !kForceUpdate );
				}
			else if ( !fPlaceholder )
				WriteHeader( kFirstWrite );	//	mark writing of the storage as incomplete
			}
		else if ( kReadWrite == eOpenMode )
			{
			//	Operate in mode 3. Use the existing stream to back our operations.
			m_pParentStorage = pStorage;		//	use existing file

			//	If we are opening an existing stream which has data then read the index
			//	else prep the empty stream for use by writing the initial header.
			if ( 0 < pStorageStream->GetSize() )
				{
				//	Remember to not write the index or aux data for this chunk since it already exists
				//	since were opening an existing file
				//	or if its a placeholder since we dont care.
				SetFlag( m_yRootFlags, kIndexWritten );
				ClearFlag( m_yRootFlags, kWriteClosingIndex );
				ClearFlag( m_yRootFlags, kWriteAuxDataOnClose );

				SetFlag( m_yRootFlags, kOpenedExisting );
				ReadChunkIndex( !kForceUpdate );
				}
			else if ( !fPlaceholder )
				WriteHeader( kFirstWrite );	//	mark writing of the storage as incomplete
			}
		else
			{
			//	Operate in mode 2. Protect the existing stream and work in a temp file.

			//	were opening to save; make a temp file for our new data; leave existing data alone; 
			//	keep existing file open while we work so that no one changes it
			XPlatformFileID xpID;

			m_prExistingStorage = pStorage;		//	keep existing storage open so no one changes it before we safe-save it later
			
			RFileStream* pExistingFile = dynamic_cast<RFileStream*>( m_prExistingStorage->GetStorageStream() );
			
			//	If we are given a file stream base the temp file on it
			//	otherwise let create temp file below make a useful temp file from
			//	an empty file id.
			if ( pExistingFile )
				pExistingFile->GetFileIdentifier( xpID );			//	get the location of the file to save

			RFileStream* prfStream = NULL;
			//	now make the storage we will use for our document
			try
				{
				prfStream = new RFileStream;
				//	open temp file so we can safe save later
				//	remember filename so we can name this file when we close
				//	allocate a temp file to back the chunk tree 
				
				//	if we fail to create the parent storage make sure we delete the stream backing it
				//	create a temp file in the same place as the given file
				prfStream->CreateTempFile( xpID );
				prfStream->Open( eOpenMode );
				m_pParentStorage = new RStorage( prfStream );
				}
			catch(...)
				{
				//	if we catch an exception opening a read/write file
				//	we need to delete the file on close
				if( (NULL != prfStream) && (kRead != eOpenMode) )
					prfStream->DeleteOnClose( TRUE );
				delete prfStream;
				throw;
				}

			//	remember to delete the temp file when were done with it
			m_pParentStorage->DeleteStreamOnClose();
			SetFlag( m_yRootFlags, kDeleteStorageOnClose );

			YStreamLength yLength = prfStream->GetSize();

			//	if were creating a new archive or the existing file was empty add the header for this chunk
			if ( 0 == yLength )	
				{
				WriteHeader( kFirstWrite );	//	mark writing of the storage as incomplete
				}
			else
				{
				//	remember that this chunk storage already exists
				//	(were opening an existing file)
				SetFlag( m_yRootFlags, kOpenedExisting );
				}
			SetFlag( m_yRootFlags, kReplaceExisting ); 
			}
		}
	catch(...)
		{	
		m_prExistingStorage = NULL;		//	If we don't do this, we'll double-destruct
		DeleteMembers();
		throw;
		}
	}
	
// ****************************************************************************
//
//  Function Name:	RChunkStorage::DeleteMembers( )
//
//  Description:		cleans out member variables. called from constructors only
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChunkStorage::DeleteMembers( )
	{
	//	Delete our member ptrs.
	//	Don't throw from here. Were already cleaning up some problem and 
	//	our caller must handle it its way not ours.
	try
		{
		if ( FlagIsSet( m_yRootFlags, kDeleteStorageOnClose ) )
			{
			delete m_pParentStorage;
			m_pParentStorage = NULL;
			}
		}
	catch( ... )
		{
		if ( FlagIsSet( m_yRootFlags, kDeleteStorageOnClose ) )
			m_pParentStorage = NULL;			//	better to leak than double destruct
		}

	try
		{
		delete m_prExistingStorage;
		m_prExistingStorage = NULL;
		}
	catch( ... )
		{
		m_prExistingStorage = NULL;			//	better to leak than double destruct
		}

	try
		{
		delete m_pAuxData;
		m_pAuxData = NULL;
		}
	catch( ... )
		{
		m_pAuxData = NULL; 						//	better to leak than double destruct
		}
	}
	
// ****************************************************************************
//
//  Function Name:	RChunkStorage::~RChunkStorage( )
//
//  Description:		dtor.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChunkStorage::~RChunkStorage( )
	{

	//	Check to see if file is read only and, if so, don't try
	//	to write anything on close (such as indexes). This should
	//	speed the destructor on many readonly files and also help
	//	fix bug 2966 when we're cleaning up a failed save (BDR).
	BOOLEAN	fWriteOnClose = FALSE;		//	are we able to write data on close?
	RStream* pStream = m_pParentStorage->GetStorageStream();
	if ( NULL != pStream )
		{
		if( kRead != pStream->GetAccessMode() )
			fWriteOnClose = TRUE;
		}
	
	
	try
		{
		try
			{
			//	install aux data in our chunk
			//	update our index
			if ( fWriteOnClose && FlagIsSet( m_yRootFlags, kWriteClosingIndex ) )	//	we wrote an initial header; write its index
				WriteIndex( );
			}
		catch( ... )
			{
			NULL;	//	can't throw from dtor so do nothing here
			}

		//	delete child storages accessed via indices
		RCInfoList::YIterator	rIter;
#if 1
		//	This will delete all of the chunks that have been demand loaded by the caller
		RCInfoList::YIterator	rIterEnd	= m_ChunkInfoList.End( );
		for ( rIter = m_ChunkInfoList.Start( ); rIter != rIterEnd; ++rIter )
#else
		//	This will insure that ALL chunks are deleted.  This will cause all chunks
		//		that have not be loaded to be loaded just so that they can be deleted.
		for( GetChunkListStart(  rIter  ); rIter != m_ChunkInfoList.End( ); ++rIter )
#endif
			if ( FlagIsSet( rIter[0].yChunkFlags, kDeleteChunkPtrOnClose) )
				delete rIter[0].pChunk;
			else
				delete rIter[0].pChunk;
		m_ChunkInfoList.Empty();

		//	append aux data to chunk data
		if ( m_pAuxData && fWriteOnClose && FlagIsSet( m_yRootFlags, kWriteAuxDataOnClose) )	
			{
			try
				{
				//	we must explicitly write our index since 
				//	our chunk is being copied before it is destructed (when its index would normally be written)
				m_pAuxData->WriteIndex();		

				YStreamPosition yPosUnused;
				RStorage* pDestStor;
				RStorage* pSrcStor;
				GetStorageAndPos( yPosUnused, pDestStor );
				m_pAuxData->GetStorageAndPos( yPosUnused, pSrcStor );
				
				YStreamLength yLen = pDestStor->GetStorageStream()->GetSize();

				pSrcStor->SeekAbsolute(0);							//	start aux data at beginning
				pDestStor->SeekAbsolute( yLen );					//	move destination stream to end
				pDestStor->Copy( pSrcStor->GetStorageStream() );
				}
			catch( ... )
				{
				NULL;	//	can't throw from dtor so do nothing here
				}
			}

		//	if we're the root do our safe save
		if ( FlagIsSet( m_yRootFlags, kReplaceExisting ) )
			{
			if ( NULL != m_prExistingStorage )
				{
				RFileStream* pStream = dynamic_cast<RFileStream*>( m_pParentStorage->GetStorageStream() );
				RFileStream* pExistingFile = dynamic_cast<RFileStream*>( m_prExistingStorage->GetStorageStream() );
				
				TpsAssert( pStream && pExistingFile, "invalid storage for safe save specified when chunk tree was created; the stream must be renameable" );
				XPlatformFileID xpID;

				pExistingFile->GetFileIdentifier( xpID );		//	close the original
				pExistingFile->Close();
				pStream->Close();
				pStream->Rename( xpID );								//	move our data into original
				}
			}
		}
	catch( ... )
		{
		NULL;		//	don't throw an exception from our dtor
		}

	try
		{
		//	if were the root delete our children
		if ( FlagIsSet( m_yRootFlags, kDeleteStorageOnClose ) )
			{
			delete m_pParentStorage;
			m_pParentStorage = NULL;
			}
		}
	catch( ... )
		{
		if ( FlagIsSet( m_yRootFlags, kDeleteStorageOnClose ) )
			m_pParentStorage = NULL;		//	better to leak than cause a double destruct
		}

	try
		{
		delete m_pAuxData;
		m_pAuxData = NULL;
		}
	catch( ... )
		{
		m_pAuxData = NULL;					//	better to leak than cause a double destruct
		}

	try
		{
		delete m_prExistingStorage;
		m_prExistingStorage = NULL;
		}
	catch( ... )
		{
		m_prExistingStorage = NULL;		//	better to leak than cause a double destruct
		}
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::Commit( )
//
//  Description:		flush the storage to disk.
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::Commit( BOOLEAN fFlush/*= FALSE*/ )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );

	//	walk the chunk tree and update its data in our stream
	if ( fFlush )
		{
		RChunkStorage* pRoot = GetFirstChunk();
		RChunkSearcher rSearcher = pRoot->Start( kRecursive );

		for(; !rSearcher.End(); ++rSearcher )
			(*rSearcher).WriteIndex();
		}

	//	append aux data to chunk data
	if ( fFlush || ( m_pAuxData && FlagIsSet( m_yRootFlags, kWriteAuxDataOnClose) ) )	
		{
		//	we must explicitly write our index since 
		//	our chunk is being copied before it is destructed (when its index would normally be written)
		m_pAuxData->WriteIndex();		

		YStreamPosition yPosUnused;
		RStorage* pDestStor;
		RStorage* pSrcStor;
		GetStorageAndPos( yPosUnused, pDestStor );
		m_pAuxData->GetStorageAndPos( yPosUnused, pSrcStor );
		
		YStreamLength yLen = pDestStor->GetStorageStream()->GetSize();

		pSrcStor->SeekAbsolute(0);							//	start aux data at beginning
		pDestStor->SeekAbsolute( yLen );					//	move destination stream to end
		pDestStor->Copy( pSrcStor->GetStorageStream() );
		}

	//	if we're the root and we should then do our safe save if the data
	//	hasn't changed since the last save
	if ( FlagIsSet( m_yRootFlags, kReplaceExisting ) )
		{
		//	flush the temp file backing the stream
		RStream* pStream = m_pParentStorage->GetStorageStream();	
		if ( NULL != pStream )
			{
			pStream->Flush();
			}

		if ( NULL != m_prExistingStorage )
			{
			TpsAssertIsObject( RFileStream, m_prExistingStorage->GetStorageStream() );
			RFileStream* pExistingFile = static_cast<RFileStream*>( m_prExistingStorage->GetStorageStream() );
			
			pExistingFile->Close();	
			pExistingFile->Delete();				//	get rid of original
			pStream->SeekAbsolute( 0 );
			pExistingFile->Open( kReadWrite );	
			pExistingFile->Copy( pStream );		//	move our data into original; keep our data
			pExistingFile->Close();	
			}
		}
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetFirstChild( )
//
//  Description:		return:
//
//  Returns:			return first child of chunk
//
//  Exceptions:		File
//
// ****************************************************************************
//
YChunkStorageId RChunkStorage::GetFirstChunkChild( )
	{
	if ( !GetChunkListCount() )
		return NULL;
	else	
		{
		RCInfoList::YIterator rIter;
		GetChunkListStart(  rIter  );
		return rIter[0].pChunk;
		}
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetFirstChunk( )
//
//  Description:		return:
//
//  Returns:			return first chunk in chunk tree
//
//  Exceptions:		File
//
// ****************************************************************************
//
YChunkStorageId RChunkStorage::GetFirstChunk( )
	{
	YStreamPosition yPosIgnored;
	return GetFirstChunk( yPosIgnored );
	}

YChunkStorageId RChunkStorage::GetFirstChunk( YStreamPosition& yPos )
	{
	YChunkId	yIndexInParent = m_yIndexInParent;
	RChunkStorage* pChunk = NULL;
	RChunkStorage* pRootStorage = this;
	RCInfoList::YIterator rIter;

	//	walk up tree of chunks looking for the root (which has a storage stream)
	//	and accumulating chunk offsets
	//	the parents are stored as RStorages so we must cast; the cast will fail when we try to
	//	cast the root (which contains the stream) and we will drop out of the loop
	//	we will also drop out of our loop if the storage contains a stream
	
	yPos = m_ChunkHeader.yHeaderSize;	//	include offset of m_pParentStorage
	for (pChunk = dynamic_cast<RChunkStorage*>(m_pParentStorage); 
		(NULL != pChunk) && (NULL == pChunk->GetStorageStream()); 
		pChunk = dynamic_cast<RChunkStorage*>(pChunk->m_pParentStorage))
		{
		//	accumulate offsets until we know the distance from the beginning 
		//	of the stream to this chunk
		TpsAssert( kInvalidChunkIndex != yIndexInParent, "Invalid Chunk found while looking for root of storage tree; no chunk selected" );
		pChunk->GetChunkListStart(  rIter  );
		yPos += (rIter[ yIndexInParent ].yChunkPos + pChunk->m_ChunkHeader.yHeaderSize);
		pRootStorage =  pChunk;
		yIndexInParent = pChunk->m_yIndexInParent;	//	remember where to look in parent on next pass
		}

	//	make sure the returned position is inside the stream
	TpsAssert( !pRootStorage || ((yPos - m_ChunkHeader.yHeaderSize) <= pRootStorage->m_pParentStorage->GetStorageStream()->GetSize()), "start of chunk after end of parent stream!" );

	TpsAssert( 0 == (yPos % kChunkAlign), "chunk header location missaligned" );
	//	return the storage
	return pRootStorage;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetStorageAndPos( YStreamPosition& yPos, RStorage*& pStorage )
//
//  Description:		Walk tree of chunks looking for the root and accumulating offsets that
//								specify the base of this chunk in the storage
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::GetStorageAndPos( YStreamPosition& yPos, RStorage*& pRootStorage )
	{
	YChunkId	yIndexInParent = GetDataParentIndex();
	RChunkStorage* pChunk = NULL;
	pRootStorage = this;
	RCInfoList::YIterator rIter;

	//	walk up tree of chunks looking for the root (which has a storage stream)
	//	and accumulating chunk offsets
	//	the parents are stored as RStorages so we must cast; the cast will fail when we try to
	//	cast the root (which contains the stream) and we will drop out of the loop
	//	we will also drop out of our loop if the storage contains a stream
	
	yPos = m_ChunkHeader.yHeaderSize;	//	include offset of m_pParentStorage
	for (pChunk = dynamic_cast<RChunkStorage*>(GetDataParent()); 
		(NULL != pChunk) && (NULL == pChunk->GetStorageStream()); 
		pChunk = dynamic_cast<RChunkStorage*>(pChunk->GetDataParent()))
		{
		//	accumulate offsets until we know the distance from the beginning 
		//	of the stream to this chunk
		TpsAssert( kInvalidChunkIndex != yIndexInParent, "Invalid Chunk found while looking for root of storage tree; no chunk selected" );
		pChunk->GetChunkListStart(  rIter  );
		yPos += (rIter[ yIndexInParent ].yChunkPos + pChunk->m_ChunkHeader.yHeaderSize);
		pRootStorage =  pChunk;
		yIndexInParent = pChunk->GetDataParentIndex();	//	remember where to look in parent on next pass
		}

	//now get the root storage from the root chunk
	pChunk = dynamic_cast<RChunkStorage*>(pRootStorage);
	TpsAssert( NULL != pChunk, "invalid chunk tree found at root" );
	pRootStorage = pChunk->GetDataParent();

		//	make sure the returned position is inside the stream
	TpsAssert( (NULL != pRootStorage) || (NULL != pRootStorage->GetStorageStream()), "Invalid chunk found" );
	TpsAssert( !pRootStorage || ((yPos - m_ChunkHeader.yHeaderSize) <= pRootStorage->GetStorageStream()->GetSize()), "start of chunk after end of parent stream!" );
	TpsAssert( 0 == (yPos % kChunkAlign), "chunk header location missaligned" );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::StorageDirty( )
//
//  Description:		set the chunk trees dirty attribute so that the storage
//								knows to do a safe save to update its contents
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::StorageDirty( )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	SetFlag( m_yRootFlags, kChunkDirty );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::DeleteBackstore( )
//
//  Description:		delete the global backstore
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::DeleteBackstore( )
	{
	delete m_pBackstore;
	m_pBackstore = NULL;		//	since this is a global we must clear it in case it is used again
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::StreamExists( )
//
//  Description:		return whether the stream backing our storage exists
//
//  Returns:			^
//
//  Exceptions:		File
//
// ****************************************************************************
//
BOOLEAN RChunkStorage::StreamExists( ) const
	{
	return m_pParentStorage->GetStorageStream()->StreamExists();
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::Grow( )
//
//  Description:		move chunk to backstore so that we may write past the location
//								of its index
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::Grow( const YStreamLength yNewSize )
	{
	//	We only support the backstore on the mac. Windows VM obviates the need for it.
	//	If we attempt to grow into the backstore on the mac then throw.

	//	REVIEW ESV 1/29/97 - Implement this when the text and headline components have been modified to not need the backstore.
//	WinCode( ThrowFileException( kWriteFailure ) );

	RChunkBackstore* pBackstore = GetRootBackstore( );
	
	//	duplicate our index in the backstore
	pBackstore->Copy( GetIndex(), yNewSize );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::Save( RChunkStorage& rDest )
//
//  Description:		save the chunk tree into the indicated storage
//
//  Returns:			none
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::Save( RChunkStorage& rDest )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	RChunkPtrList rChunkList;				//	list of indices left to be added to chunk tree

	//	finish with writing the given chunk
	WriteIndex();

	//	remove existing indices from this chunk
	RChunkPtrList::YIterator rIter;
	RChunkPtrList::YIterator rChildIter;
	
	PUBYTE pubData = NULL;
	YStreamLength yDataSize;
	YStreamPosition yOldPos;

	//	we are the first chunk
	RChunkStorage *pSavedSelection;
	rChunkList.InsertAtEnd( this );

	//	perform depth first search of our chunk tree; search down until we have found all chunks
	//	i.e. search children first then siblings
	//	remember where we added subtrees with null marker list entries so we may reconstruct
	//	the structure of the source tree in the destination by starting new destination subtrees
	//	in their correct locations relative to the source tree
	for ( rIter = rChunkList.Start()
		; rChunkList.Count() && (rIter != rChunkList.End())
		;)
		{
		//	if we find a marker chunk (which has a NULL data ptr) then select our parent
		//	and remove the marker
		while ( (NULL == (*rIter)) && (0 != rChunkList.Count()) )
			{
			//	stop at the root
			if ( !rDest.GetSelectedChunk()->IsRoot() )
				rDest.SelectParentChunk();
			rChunkList.RemoveAt( rIter );
			--rIter;
			}
		
		if ( 0 == rChunkList.Count() )
			break;
		
		//	override the selection mechanism for the current chunk
		pSavedSelection = GetSelectedChunk();
		SetSelectedChunk( (*rIter) );
		
		if ( !(*rIter)->IsRoot() )
			{
			//	save the current chunk in the destination archive from this chunk tree
			YChunkInfo &yIndex = (*rIter)->GetIndex();
			rDest.AddChunk( yIndex.yChunkTag );

			//	save the chunk's data
			yDataSize = GetDataSize();
			if ( 0 < yDataSize )
				{
				yOldPos = GetPosition();
				try
					{
					pubData = new uBYTE[ yDataSize ];
					SeekAbsolute( 0 );
					Read( yDataSize, pubData );
					rDest.Write( yDataSize, pubData );
					SeekAbsolute( yOldPos );
					}
				catch(...)
					{
					delete[] pubData;
					throw;
					}
				delete[] pubData;
				pubData = NULL;
				}

			//	save the chunk's aux data
			if ( HasAuxData() )
				{
				yDataSize = GetAuxDataSize();
				try
					{
					pubData = new uBYTE[ yDataSize ];
					ReadAuxData( yDataSize, pubData );
					rDest.WriteAuxData( yDataSize, pubData );
					}
				catch(...)
					{
					delete[] pubData;
					throw;
					}
				delete[] pubData;
				pubData = NULL;
				}
			//	write marker to tell destination chunk that we need to add the next chunk from our parent
			rChunkList.InsertAtEnd( NULL );
			}
		
		//	insert children at the end of the list
		{
		RCInfoList &rIndexList = GetSelectedChunk()->m_ChunkInfoList;
		YChunkId yNumChildren = (*rIter)->GetNumberOfChunks();
// GCB 10/10/96 - fix Mac compile error "cannot convert RArray<YChunkInfo>::RArrayIterator to RArray<RChunkStorage*>::RArrayIterator"
// and rChildIter not used anyway
#if 0
		rChildIter = rIndexList.Start();
#else
		rIndexList.Start();
#endif
		for( ; 0 < yNumChildren; --yNumChildren )
			{
			//	add the child
#ifdef MAC // GCB 10/10/96 fix Mac compile error "pointer/array required" !?
			rChunkList.InsertAtEnd( ((YChunkInfo * const)rIndexList)[ yNumChildren-1 ].pChunk );
#else
			rChunkList.InsertAtEnd( rIndexList[ yNumChildren-1 ].pChunk );
#endif
			}
		}

		SetSelectedChunk( pSavedSelection );
		
		rChunkList.RemoveAt( rIter );	//	were done with this chunk
		rIter = rChunkList.End();		//	go on to next chunk
		--rIter;
		}
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::ReleaseBuffer( PUBYTE pubBuffer ) const
//
//  Description:		release given buffer which was allocated by GetCurrentChunkBuffer
//
//  Returns:			none
//
//  Exceptions:		none
//
// ****************************************************************************
//
void RChunkStorage::ReleaseBuffer( PUBYTE pubBuffer ) const
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	YStreamPosition yChunkSrcOffset;
	RStorage* pSrcStor;
	m_pSelectedStorage->GetStorageAndPos( yChunkSrcOffset, pSrcStor);
	RStream* pSrcStream = pSrcStor->GetStorageStream();
	pSrcStream->ReleaseBuffer( pubBuffer );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetCurrentChunkDataBuffer( YStreamLength& yBufferSize ) const
//
//  Description:		return a ptr to a buffer containing the contents of this chunk
//								and the size of the buffer in the yBufferSize parameter
//
//  Returns:			^
//
//  Exceptions:		File
//
// ****************************************************************************
//
PUBYTE RChunkStorage::GetCurrentChunkDataBuffer( YStreamLength& yBufferSize ) const
	{
	TpsAssert( !m_pSelectedStorage->IsRoot(), "The root has no index and so has no local data. The returned data is invalid." );

	//	NOTE:
	//	GetCurrentChunkDataBuffer was provide for development use only. 
	//	Use of this api under windows provides a performance benefit. 
	//	If you want the data contents of a chunk for other purposes then use 
	//	Read to get it. The mac implementation would allocate a heap block 
	//	and copy in the chunk contents so do not plan on using this api 
	//	for cross platform development purposes.

#ifdef _WINDOWS
	PUBYTE pubBuffer = GetCurrentChunkBuffer( yBufferSize ) + m_ChunkHeader.yHeaderSize;
	
	//	GetCurrentChunkBuffer returns the size of the entire chunk. We only want the size of it's data.
	yBufferSize = GetDataSize( );
	return pubBuffer;
#else
	UnimplementedCode();
	return NULL;
#endif
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetCurrentChunkBuffer( YStreamLength& yBufferSize ) const
//
//  Description:		return a ptr to a buffer containing the contents of this chunk
//								including its header (so that it can be reconstructed as a chunk later)
//								and the size of the buffer in the yBufferSize parameter
//
//  Returns:			^
//
//  Exceptions:		File
//
// ****************************************************************************
//
PUBYTE RChunkStorage::GetCurrentChunkBuffer( YStreamLength& yBufferSize ) const
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
	
	//	make sure buffer contains updated information for chunk lists, etc
	m_pSelectedStorage->WriteIndex();

	//	Get the stream containing this tree and 
	//	figure out where the selected chunk starts in the stream.
	//	Get the start of the chunk's data in yChunkSrcOffset.
	YStreamPosition yChunkSrcOffset;
	RStorage* pSrcStor;	
	m_pSelectedStorage->GetStorageAndPos( yChunkSrcOffset, pSrcStor );
	RStream* pSrcStream = pSrcStor->GetStorageStream();

	//	Get the chunk's size and adjust its start position in the stream.
	if ( m_pSelectedStorage->IsRoot() )
		{
		//	The root has no parent (and so no index) so use the entire stream.
		//	Use the entire chunk tree stream if were using the root.
		//	Start at the beginning of the stream.
		yBufferSize = pSrcStream->GetSize();
		yChunkSrcOffset = 0;
		}
	else
		{
		//	Our index contains our total size (including our children and our data). Get our size.
		yBufferSize = m_pSelectedStorage->GetIndex().yChunkSize;
		
		//	Get the beginning of the chunk; back the base offset (to the chunk's data) returned
		// from GetStorageAndPos back to beginning of the chunk's header.
		TpsAssert( yChunkSrcOffset > m_pSelectedStorage->m_ChunkHeader.yHeaderSize, "invalid chunk position" );
		yChunkSrcOffset -= m_pSelectedStorage->m_ChunkHeader.yHeaderSize;
		}

	//	Return a pointer to the beginning of the selected chunk.
	//	Use GetBuffer to get the memory and fill it with the chunk's data.
	//	(On Windows the memory is already allocated after the memory mapped file's base ptr)
	//	GetBuffer operates relative to the current stream position so preposition the stream
	//	marker to the beginning of the chunk.
	PUBYTE pubChunkPos;
	YStreamPosition yOldPosition = pSrcStream->GetPosition();
	pSrcStream->SeekAbsolute( yChunkSrcOffset );
	pubChunkPos = pSrcStream->GetBuffer( yBufferSize );
	pSrcStream->SeekAbsolute( yOldPosition );

	return pubChunkPos;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::IsInBackstore(  )
//
//  Description:		return TRUE if the chunk is in the backstore
//
//  Returns:			^
//
//  Exceptions:		File
//
// ****************************************************************************
//
BOOLEAN RChunkStorage::IsInBackstore( )
	{
	RChunkBackstore* pBackstore = GetRootBackstore( !kCreateBackstoreIfNecessary );
	
	//	If we have a backstore then check it for our storage.
	if ( pBackstore )
		{
		RStorage* pStor;
		RStorage* pBackStor;
		YStreamPosition yUnused;

		pBackstore->GetStorageAndPos( yUnused, pStor);
		GetStorageAndPos( yUnused, pBackStor);

		return static_cast<BOOLEAN>(pBackStor == pStor);
		}
	else 
		//	If we have no backstore then we are not in it.
		return FALSE;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetCurrentFileSystemVersion(  )
//
//  Description:		return kCurrentRenaissanceFileSystemVersion
//
//  Returns:			^
//
//  Exceptions:		none
//
// ****************************************************************************
//
YVersion RChunkStorage::GetRenaissanceFileSystemVersion( )
	{
	return kCurrentRenaissanceFileSystemVersion;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetCurrentChunkMinFileSystemVersion(  )
//
//  Description:		return the version of the file system required for the current chunk
//
//  Returns:			^
//
//  Exceptions:		none
//
// ****************************************************************************
//
YVersion& RChunkStorage::GetCurrentChunkMinFileSystemVersion( )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	return m_pSelectedStorage->m_ChunkHeader.yvFileSystemMinimumVersion;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetCurrentChunkCurrentFileSystemVersion(  )
//
//  Description:		return the version of the file system used when the current chunk was created
//
//  Returns:			^
//
//  Exceptions:		none
//
// ****************************************************************************
//
YVersion& RChunkStorage::GetCurrentChunkCurrentFileSystemVersion( )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	return m_pSelectedStorage->m_ChunkHeader.yvFileSystemCurrentVersion;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetCurrentChunkMinDataVersion(  )
//
//  Description:		return the version of the file system required for the 
//							current chunk's data
//
//  Returns:			^
//
//  Exceptions:		none
//
// ****************************************************************************
//
YVersion& RChunkStorage::GetCurrentChunkMinDataVersion( )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	return m_pSelectedStorage->m_ChunkHeader.yvDataMinimumVersion;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetCurrentChunkCurrentDataVersion(  )
//
//  Description:		return the version of the file system used when the current 
//							chunk's data was created
//
//  Returns:			^
//
//  Exceptions:		none
//
// ****************************************************************************
//
YVersion& RChunkStorage::GetCurrentChunkCurrentDataVersion( )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	return m_pSelectedStorage->m_ChunkHeader.yvDataCurrentVersion;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::Copy( const YChunkInfo& yci, YStreamLength yNewSize )
//
//  Description:		move the chunk refered to by the index into the backstore
//							and fix up its references
//
//  Returns:			none
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::Copy( const YChunkInfo& yci, YStreamLength yNewSize )
	{
	//	get the chunk were adding
	RChunkStorage* pCurrentSelection = m_pSelectedStorage;		
	RChunkStorage* pAddChunk = yci.pChunk;		
	
	RStorage* pSrcStor;
	YStreamPosition yChunkSrcOffset;
	pAddChunk->GetStorageAndPos( yChunkSrcOffset, pSrcStor);
	RStream* pSrcStream = pSrcStor->GetStorageStream();

	//	make the requested size a multiple of the minimum chunk size so that we have room to grow
	yNewSize = (yNewSize/kChunkMinSize + 1) * kChunkMinSize;

	//	find an available chunk that can accomodate the given size 
	BOOLEAN fFound = FALSE;
	YChunkId yBackstoreIndexId;
	
	//	loop thru backstore chunks looking for a deleted chunk that's big enough
	RCInfoList::YIterator rIter;
	GetChunkListStart(  rIter  );
//	YChunkId yNumBackstoreChunks = GetChunkListCount();
//	for(	yBackstoreIndexId = 0;
//			yBackstoreIndexId < yNumBackstoreChunks;
//			++yBackstoreIndexId )
//		if ( FlagIsSet( rIter[ yBackstoreIndexId ].yChunkFlags, kChunkDeleted ) && ( yNewSize >= rIter[ yBackstoreIndexId ].yChunkSize ) )
//				{	
//				fFound = TRUE;
//				ClearFlag( rIter[ yBackstoreIndexId ].yChunkFlags, kChunkDeleted );
//				}

	//	none found so make a new one
	//	before inserting the index we need to set it up to refer to the backstore
	if ( !fFound )
		{
#ifdef kFORCEFIXBACKSTORECHUNK
		if ( (pAddChunk->m_pParentStorage == pAddChunk->m_pBackstore)
			&& (NULL != pAddChunk->m_pDataParentStorage) )
			{
			//	if were writing the index of a backstore chunk then update its size info
			//	since this data was orphaned by the existing dual storage (DataParentStorage/ParentStorage)
			//	architecture
			TpsAssert( pAddChunk->m_yIndexInDataParent != kInvalidChunkIndex, "invalid chunk found" );
			rIter[pAddChunk->m_yIndexInDataParent].pChunk->m_ySize = pAddChunk->m_ySize;
			rIter[pAddChunk->m_yIndexInDataParent].pChunk->m_yMaxSize = pAddChunk->m_yMaxSize;
			}
#endif

		yBackstoreIndexId = AddChunk( yci.yChunkTag );
		GetChunkListStart(  rIter  );
		}
	else
		rIter[ yBackstoreIndexId ].pChunk =		pAddChunk;

	rIter[ yBackstoreIndexId ].yChunkFlags =	yci.yChunkFlags;
	rIter[ yBackstoreIndexId ].yChunkTag =		yci.yChunkTag;
	rIter[ yBackstoreIndexId ].yPlatformTag = yci.yPlatformTag;
	
	//	if the chunk was in the backstore
	//	mark it as deleted
	if ( pAddChunk->IsInBackstore( ) )
		{
		if ( pAddChunk->m_yIndexInDataParent == kInvalidChunkIndex )
			SetFlag( rIter[ pAddChunk->m_yIndexInParent ].yChunkFlags, kChunkDeleted );	
		else
			SetFlag( rIter[ pAddChunk->m_yIndexInParent ].yChunkFlags, kChunkDeleted );	
		}

	//	make future data operations on this chunk use the data storage
	pAddChunk->m_yIndexInDataParent = yBackstoreIndexId;
	pAddChunk->m_pDataParentStorage = this;
	
	//	if we are a backstore only chunk then keep our indices consistent
	if ( pAddChunk->m_pDataParentStorage == pAddChunk->m_pParentStorage )
		pAddChunk->m_yIndexInParent = yBackstoreIndexId;

	//	copy in chunk's data to the backstore; don't write if we have nothing to copy
	pSrcStream->SeekAbsolute( yChunkSrcOffset );

	if ( pAddChunk->m_ySize )
		Write( pAddChunk->m_ySize, pSrcStream->GetBuffer( pAddChunk->m_ySize ) );

	pAddChunk->m_ySize = pAddChunk->m_ySize;
	pAddChunk->m_yMarker = pAddChunk->m_ySize;
	pAddChunk->m_yMaxSize = yNewSize;
	m_pSelectedStorage->WriteHeader();

	
	//	if we aren't a backstore only chunk then 
	if ( pAddChunk->m_pDataParentStorage != pAddChunk->m_pParentStorage )
	//	continue our current operation after we move
	//	into the backstore from where we started
		m_pSelectedStorage = pCurrentSelection;	//	restore selection before we were added
	
	//	make sure we rewrite the index since its contents will now change
#ifdef kCLEARCOPIEDINDICES
	ClearFlag( pAddChunk->m_yRootFlags, kIndexWritten );	
#endif
	}




// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetStorage( RStorage* pStorage )
//
//  Description:		Return the storage used for read and write operations
//							associated with the given storage. Either the given
//							storage or the storage backing the chunk tree if the 
//							given storage is really a chunk storage.
//
//  Returns:			^
//
//  Exceptions:		File
//
// ****************************************************************************
//
RStorage* RChunkStorage::GetStorage( RStorage* pStorage )
	{
	RStorage* pStorageToCheck = pStorage;

	//	If we're an internal tree node then check the root for its storage.
	RChunkStorage *pInternalNode = dynamic_cast<RChunkStorage*>( pStorage );
	if ( pInternalNode )
		{
		YStreamPosition yUnused;
		pInternalNode->GetStorageAndPos( yUnused, pStorageToCheck );
		}
	
	return pStorageToCheck;
	}

// ****************************************************************************
//
//  Function Name:	RChunkStorage::IsChunkStorage( RStorage* pStorage )
//
//  Description:		Return TRUE if the given storage is a chunky storage.
//
//  Returns:			^
//
//  Exceptions:		File
//
// ****************************************************************************
//
BOOLEAN RChunkStorage::IsChunkStorage( RStorage* pStorage )
	{
	TpsAssert( pStorage, "Null input storage" );

	RStorage* pStorageToCheck = GetStorage( pStorage );

	//	Can't be a chunky storage if there is no stream attached.
	if ( !pStorageToCheck || !pStorageToCheck->GetStorageStream() )
		return FALSE;

	//	We must allow empty streams to be chunk storages for two reasons:
	//	- when storages are created they are frequently created with empty streams
	//	- if we ever support os creation of new empty documents (for example in the in the windows explorer) they will be empty files
	if ( 0 == pStorageToCheck->GetStorageStream()->GetSize() )
		return TRUE;

	try
		{
		//	Read the magic key from beginning of the given stream. If it matches with our 
		//	magic key then return TRUE. Replace stream pointer.
		YStreamLength yCurrentPosition = pStorageToCheck->GetPosition();
			
		//	Seek to beginning of header; Handle byte swapping; 
		YChunkHeader yHeader;
		pStorageToCheck->SeekAbsolute( 0 );
		pStorageToCheck->Read( sizeof( yHeader.ulMagicKey ) + sizeof( yHeader.yPlatformTag ) + sizeof( yHeader.yHeaderSize )
			, (PUBYTE)&yHeader );
		
		if ( yHeader.yPlatformTag != RStorage::GetPlatformTag( ) )
			yHeader.ulMagicKey = tintSwapLong( yHeader.ulMagicKey );
		
		//	Read from the beginning since the first long in the stream must be the first byte
		//	of the chunk storage's header. The first byte of the header must be the magic key.
		pStorageToCheck->SeekAbsolute( yCurrentPosition );
		
		return static_cast<BOOLEAN>(kMagicKey == yHeader.ulMagicKey);
		}
	catch( ... )
		{
		//	REVIEW ESV 2/5/97 - what else do we do here?
		return FALSE;
		}
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::ReadChunkIndex( )
//
//  Description:		load all chunks as children of this one
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::ReadChunkIndex( const BOOLEAN fForceUpdate )
	{
	//	install chunks as children of this one
	YStreamPosition yAbsoluteBase;					//	current file pointer relative to start of this chunk
	RStorage* pStorage;									//	storage for all chunks in this tree of chunks
	RChunkPtrList rChunkList;							//	list of indices left to be added to chunk tree

	//	get the storage holding the chunks to read
	GetStorageAndPos( yAbsoluteBase, pStorage );
	yAbsoluteBase -= m_ChunkHeader.yHeaderSize;	//	GetStorageAndPos return a position relative to the 
																//		end of the header; move it back
	
	//	perform breadth first search on chunk storage
	//	allocate memory chunks and indices as needed and install them in this chunk

	//	remove existing indices from this chunk
	RChunkPtrList::YIterator rIter;
	
	//	Search down until we have found all chunks or just the first one if we aren't searching recursively.
	YChunkId yCurIndex;						//	index into current chunk
	YCounter yListIndex = 0;				//	index into list of all chunk indices
	YStreamPosition yHeaderPos = 0;		//	start of current header in stream
	YStreamPosition yHeaderBasePos = 0;	//	start of current header in stream
	YChunkInfo cInfo;							//	current chunk info being read

	BOOLEAN	fRecurOnChildren = static_cast<BOOLEAN>( kLoadAllChunkIndicesRecursively || fForceUpdate );
	
	//	we are the first chunk
	//	get the base position of the chunk stream to compensate for its location in the destination
if ( !kLoadAllChunkIndicesRecursively )
	{		
	GetStorageAndPos( yHeaderBasePos, pStorage );
	yHeaderBasePos -= m_ChunkHeader.yHeaderSize;		//	GetStorageAndPos return a position relative to the 
	}

	RChunkStorage *pCurrentChunk = this;
	rChunkList.InsertAtEnd( pCurrentChunk );
	rIter = rChunkList.Start();

	//	iterate over archive; limit the number of iterations to the largest number of indices that might be in the archive
	//		so that we don't use an unlimited search loop; the largest number of indices occurs when there is nothing but 
	//		indices in the archive
	RStream* pStream = m_pParentStorage->GetStorageStream();											//	storage for all chunks in this tree of chunks
	YStreamLength yLength = 0;

	//	If were the root use the entire stream's size else use our size.
	if ( pStream )
		yLength = pStream->GetSize();
	else
		yLength = m_ySize;

	yLength /= sizeof( YChunkInfo );

	for( ; 0 < yLength; --yLength )
		{
		//	get the current chunk whose info were loading from disk
		//	and prep stream ptr for reading that chunk
		rIter = rChunkList.Start();
		pCurrentChunk = rIter[yListIndex];
		pCurrentChunk->GetStorageAndPos( yHeaderPos, pStorage );
		yHeaderPos -= m_ChunkHeader.yHeaderSize;	//	GetStorageAndPos return a position relative to the 
																//		end of the header; move it back
		if( kLoadAllChunkIndicesRecursively )
			yHeaderPos -= yHeaderBasePos;				//	subtract the starting position of the destination chunk in the destination stream
		pStorage->SeekAbsolute( yHeaderPos );

		//	Read and validate the header.
		pCurrentChunk->ReadHeader( pStorage );

		//	if we're reading the root perform additional validation on the only chunk the file system owns outright
		if ( this == pCurrentChunk )
			{
			TpsAssert( ! FlagIsSet( pCurrentChunk->m_ChunkHeader.yfHeaderFlags, kWriteIncomplete ), "chunk storage was not completely writen" );
		
			if ( pCurrentChunk->m_ChunkHeader.yvFileSystemMinimumVersion < kCurrentRenaissanceFileSystemVersion )
				ThrowFileException( kDataFormatTooOld );
		
			if ( FlagIsSet( pCurrentChunk->m_ChunkHeader.yfHeaderFlags, kWriteIncomplete ) )
				ThrowFileException( kDataFormatInvalid );
			}
		else 
			{ 
			//	use my header to calculate the size of my data (excluding my header and indices)
			pCurrentChunk->m_ySize = pCurrentChunk->m_ChunkHeader.ylChunkInfoPos - pCurrentChunk->m_ChunkHeader.ylChunkAlignPad;	
			//	we cannot grow without special action in a completely constructed chunk	
			pCurrentChunk->m_yMaxSize = pCurrentChunk->m_ySize;		
			}

		if ( fRecurOnChildren )
			{
			//	load its indices
			pStorage->SeekRelative( pCurrentChunk->m_ChunkHeader.ylChunkInfoPos );
			for( yCurIndex = 0; yCurIndex < pCurrentChunk->m_ChunkHeader.yciNumberOfChunks; ++yCurIndex )
				{
				//	read the index and add it to the list
				//	remember absolute position of chunk's data so it may be accessed out of 
				//		the context of other chunks (whose headers are not loaded when the data is read)
				pStorage->Read( sizeof( YChunkInfo ), (PUBYTE)&cInfo );
			
				// GCB 10/21/96 - handle byte swapping
				if (pCurrentChunk->m_ChunkHeader.yPlatformTag != RStorage::GetPlatformTag( ))
					{
	#ifdef _WINDOWS
				TpsAssert( pCurrentChunk->m_ChunkHeader.yPlatformTag == kMac, "reading invalid data or a chunk written for an unsupported platform" );
	#elif defined MAC
				TpsAssert( pCurrentChunk->m_ChunkHeader.yPlatformTag == kWin, "reading invalid data or a chunk written for an unsupported platform" );
	#endif
					cInfo.yChunkTag = tintSwapLong( cInfo.yChunkTag );
					cInfo.yChunkId = tintSwapLong( cInfo.yChunkId );
					cInfo.yChunkFlags = tintSwapLong( cInfo.yChunkFlags );
					cInfo.yChunkPos = tintSwapLong( cInfo.yChunkPos );
					cInfo.yChunkSize = tintSwapLong( cInfo.yChunkSize );
					}

				Debug( ThrowFileExceptionIfTesting( kDataFormatInvalid ) );

				SetFlag( cInfo.yChunkFlags, kDeleteChunkPtrOnClose);
				cInfo.pChunk = new RChunkStorage;										//	make the chunk this index refers to and remember it for later

				SetFlag( cInfo.pChunk->m_yRootFlags, kIndexWritten );				//	the file was opened for read access; don't write anything
				cInfo.pChunk->m_pParentStorage = pCurrentChunk;
				cInfo.pChunk->m_yIndexInParent = yCurIndex;
				ClearFlag( cInfo.pChunk->m_yRootFlags, kWriteClosingIndex );
				
				//	Only recur on the new chunk if were loading all indices at once.
				if ( fRecurOnChildren )
					rChunkList.InsertAtEnd( cInfo.pChunk );

				pCurrentChunk->m_ChunkInfoList.InsertAtEnd( cInfo );
				}
			
			//	Only keep loading indices after the first set if were going to load them all.
			fRecurOnChildren = kLoadAllChunkIndicesRecursively;
			}

		//	reposition to read next header
		//	stop when we have read all the indices or the remaining index is emtpy
		//	use listcout - 1 since the first list entry has already been processed
		if ( (rChunkList.Count() - 1) <= yListIndex )
			break;	//	were done
		else
			++yListIndex;	//	goto next chunk to load
		}

	try
		{
		//	if this assert fires the input stream is poorly formatted; probably not a renaissance file
		//		we though we were loading a chunk but we iterated over the data more times than we could have needed
		//		to without finding a well formed chunk tree
		TpsAssert( yLength, "Invalid chunk found; loading terminated before stream exhausted" );
		Debug( ThrowFileExceptionIfTesting( kDataFormatInvalid ) );
		if ( 0 == yLength )
			ThrowFileException( kDataFormatInvalid );
		
		//	Read in the aux data; the source stream should be at the beginning of the aux data.
		//	If were reading the aux data's index then stop ( the aux data doesn't have an aux data).
		if ( fRecurOnChildren && m_pAuxData && ( kLoadAllChunkIndicesRecursively || (0 == yHeaderBasePos) ))	
			{
			YStreamPosition yPosUnused;
			RStorage* pDestStor;
			RStorage* pSrcStor;
			GetStorageAndPos( yPosUnused, pSrcStor );
			m_pAuxData->GetStorageAndPos( yPosUnused, pDestStor );

			RChunkStorage* pRootChunk = GetFirstChunk( );

			//	find start of aux data in stream
			
			//	remember that there is one index for chunks with no children
			YChunkId yNumIndices = Max( 1UL, pRootChunk->GetNumberOfChunks() );
			
			yHeaderPos = pRootChunk->m_ySize + pRootChunk->m_ChunkHeader.yHeaderSize 
				 + pRootChunk->m_ChunkHeader.ylChunkAlignPad + yNumIndices * sizeof( YChunkInfo );
			pSrcStor->SeekAbsolute( yHeaderPos );					//	move destination stream marker to start
			pDestStor->SeekAbsolute( 0 );								//	move destination stream marker to start
			pDestStor->Copy( pSrcStor->GetStorageStream() );	//	load aux data into aux data chunk
			m_pAuxData->ReadChunkIndex( kForceUpdate );			//	read the chunk headers into memory
			}
		}
	catch( ... )
		{
		//	if were throw here we have to delete the allocated list entries since
		//	we will end up in the ctor and the dtor (which normally would delete
		//	these) won't be called
		//	the first entry is this object so it will be deleted
		rIter = rChunkList.Start();
		for( ++rIter; rIter != rChunkList.End(); ++rIter )
			delete (*rIter);

		throw;
		}
	}

// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetDataParent( ) const
//
//  Description:		return our parent as used for reading and writing data
//
//  Returns:			^
//
//  Exceptions:		none
//
// ****************************************************************************
//
RStorage* RChunkStorage::GetDataParent( ) const
	{
	if ( NULL != m_pDataParentStorage )
		{
		TpsAssert( kInvalidChunkIndex != m_yIndexInDataParent, "invalid index in backstore" );
		return m_pDataParentStorage;
		}
	else
		return m_pParentStorage;
	}

// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetDataParentIndex( ) const
//
//  Description:		return our index in our parent as used for reading and writing data
//
//  Returns:			^
//
//  Exceptions:		none
//
// ****************************************************************************
//
YChunkId RChunkStorage::GetDataParentIndex( ) const
	{
	if ( NULL != m_pDataParentStorage )
		{
		TpsAssert( kInvalidChunkIndex != m_yIndexInDataParent, "invalid index in backstore" );
		return m_yIndexInDataParent;
		}
	else
		return m_yIndexInParent;
	}

// ****************************************************************************
//
//  Function Name:	RChunkStorage::UpdateParentSize( BOOLEAN fSetMaxSize = FALSE )
//
//  Description:		Walk tree of chunks updating sizes based on size of children
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::UpdateParentSize( BOOLEAN fSetMaxSize /*= FALSE*/ )
	{
	TpsAssert( !IsRoot(), "the root has no parent so its size cannot be updated" );

	YChunkId	yIndexInParent = GetDataParentIndex();
	RCInfoList::YIterator rParentIter;
	RCInfoList::YIterator rChildIter;
	RChunkStorage *pChunk = dynamic_cast<RChunkStorage*>(GetDataParent());
	YChunkId yNumIndices = m_ChunkHeader.yciNumberOfChunks;

	if ( 0 == yNumIndices )	//	we always write a dummy index even if the chunk has no children
		yNumIndices = 1;

	//	our size has changed; 
	//	update our size in our index in our parent
	//	we assume we will not write in a chunk after we have written its index
	//	our size includes our indices so that our parents know where to write their next child or index
	pChunk->GetChunkListStart( rParentIter );
	rParentIter[yIndexInParent].yChunkSize = m_ySize + m_ChunkHeader.yHeaderSize + m_ChunkHeader.ylChunkAlignPad 
		+ yNumIndices * sizeof( YChunkInfo );

	//	index is after all sub-chunks; pad data to align chunk
	m_ChunkHeader.ylChunkAlignPad = kChunkAlign - m_ySize % kChunkAlign;
	if ( kChunkAlign == m_ChunkHeader.ylChunkAlignPad )
		m_ChunkHeader.ylChunkAlignPad = 0;					//	don't pad if the data is already aligned
	m_ChunkHeader.ylChunkInfoPos = m_ySize + m_ChunkHeader.ylChunkAlignPad;

	//	fix up our parents one at a time
	for (pChunk = dynamic_cast<RChunkStorage*>(GetDataParent());
		(NULL != pChunk) && (NULL == pChunk->GetStorageStream()); 
		pChunk = dynamic_cast<RChunkStorage*>(pChunk->GetDataParent()))
		{
		//	chunk's size is the sum of its data plus its childen's sizes
		pChunk->m_ySize = pChunk->m_ChunkHeader.ylChunkDataSize;

		for( pChunk->GetChunkListStart( rChildIter ); rChildIter != pChunk->m_ChunkInfoList.End( ); ++rChildIter )
			pChunk->m_ySize += ( rChildIter[0].yChunkSize );
		
		//	make sure we don't write past our indices by setting our max size
		if ( fSetMaxSize )
			pChunk->m_yMaxSize = pChunk->m_ySize;

		//	if our parent is a chunk then 
		RChunkStorage *pChunksParent = dynamic_cast<RChunkStorage*>(pChunk->GetDataParent()); 

		if ( pChunksParent )
			{
			TpsAssert( kInvalidChunkIndex != pChunk->GetDataParentIndex(), "Invalid Chunk found" );
			yNumIndices = pChunk->m_ChunkHeader.yciNumberOfChunks;

			if ( 0 == yNumIndices )	//	we always write a dummy index even if the chunk has no children
				yNumIndices = 1;

			//	update chunks size in our index in chunks parent
			//	we assume we will not write in a chunk after we have written its index
			//	our indices are not included since they have not yet been written
			pChunksParent->GetChunkListStart( rParentIter );
			rParentIter[pChunk->GetDataParentIndex()].yChunkSize = pChunk->m_ySize + pChunk->m_ChunkHeader.yHeaderSize + pChunk->m_ChunkHeader.ylChunkAlignPad;
			}

		//	index is after all sub-chunks;	pad data to align chunks
		pChunk->m_ChunkHeader.ylChunkAlignPad = kChunkAlign - pChunk->m_ySize % kChunkAlign;
		if ( kChunkAlign == pChunk->m_ChunkHeader.ylChunkAlignPad )
			pChunk->m_ChunkHeader.ylChunkAlignPad = 0;					//	don't pad if the data is already aligned
		pChunk->m_ChunkHeader.ylChunkInfoPos = pChunk->m_ySize + pChunk->m_ChunkHeader.ylChunkAlignPad;

		yIndexInParent = pChunk->GetDataParentIndex();	//	remember where to look in parent on next pass

#if 0	//def TPSDEBUG
		YStreamPosition yPosUnused;
		RStorage* pSrcStor;
		GetStorageAndPos( yPosUnused, pSrcStor );
		
		TpsAssert( pSrcStor->GetStorageStream()->GetSize() < pChunk->m_ySize, "chunk larger than the entire stream" );
#endif
		}
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::Write( const uBYTE* pubBuffer, YStreamLength yBufferSize )
//
//  Description:		write given data to our stream
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::Write( YStreamLength yBufferSize, const uBYTE* pubBuffer )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	RChunkStorage* pInitialSelection = m_pSelectedStorage;

	m_pSelectedStorage->WriteInternal( yBufferSize, pubBuffer );

#ifdef kPOSTCOPYWRITEFIX
	//	fix up write to backstore if selection changed during write because a chunk moved in the backstore
	if ( (pInitialSelection != m_pSelectedStorage) )
		{
		m_pSelectedStorage->m_yMarker = pInitialSelection->m_yMarker;
		m_pSelectedStorage->m_ySize = pInitialSelection->m_ySize;
		m_pSelectedStorage->m_ChunkHeader.ylChunkDataSize = pInitialSelection->m_ChunkHeader.ylChunkDataSize;
		m_pSelectedStorage->m_ChunkHeader.ylChunkDataPos = pInitialSelection->m_ChunkHeader.ylChunkDataPos;
		}
#endif
	}

void RChunkStorage::WriteInternal( const YStreamLength yBufferSize, const uBYTE* pubBuffer )
	{
	TpsAssert( !IsRoot(), "The root chunk has no parent so it is not accessible for data operations" );

	BOOLEAN fNonContiguousWrite = FALSE;

	//	if this write will grow the stream past where its index is written in the stream for its chunk tree
	//		move it to a backstore and continue
	if ((m_yMarker + yBufferSize) > m_yMaxSize)
		Grow( m_yMarker + yBufferSize );

	if ( 0 != GetChunkListCount() )					//	if there is a child before us
		{															// then write its index
		//	if the child has already had its index written then this is unnecessary
		//	and WriteIndex() will do nothing but we must check anyway

		//	get the index of the last child in our chunk
		//	we only worry about the last child in this chunk because earlier children
		//	had their indices written when the chunk after them was added
		RCInfoList::YIterator rIter;
		GetChunkListStart(  rIter  );

		YChunkInfo& yChunkBeforeUs = rIter[ GetChunkListCount() - 1 ];
		
		//	put the new header after the data and indices of the chunk before us
		yChunkBeforeUs.pChunk->WriteIndex();
		}

	//	handle offset from beginning of chunk to beginning of chunk's data
	//	handle first write; remember where data was written
	if ( kInvalidChunkPos == m_ChunkHeader.ylChunkDataPos )
		{
		//	our local data is after chunks that have already been written
		m_ChunkHeader.ylChunkDataPos = m_ySize;
		m_yMarker = 0;		//	we haven't written yet so the marker should be  0
		}

	//	offset the operation from the beginning of the data
	m_yMarker += m_ChunkHeader.ylChunkDataPos;

	if ( FlagIsSet( GetIndex().yChunkFlags, kChunkContiguousDataDone ) && (m_yMarker > m_ChunkHeader.ylChunkDataPos) )
		{
		//	weve written before; if we are writing outside of the area we wrote in initially then assert
		TpsAssertAlways( "Writing non-contiguous chunk data" );
		fNonContiguousWrite = TRUE;
		}

	YStreamPosition yAbsoluteBase;			//	current file pointer relative to start of this chunk
	RStorage* pStorage;							//	storage for all chunks in this tree of chunks

	GetStorageAndPos( yAbsoluteBase, pStorage );

	TpsAssert( m_yMarker >= m_ChunkHeader.ylChunkDataPos, "overwriting child in chunk; writing before start of chunks data area" );
	pStorage->SeekAbsolute( yAbsoluteBase + m_yMarker );
	pStorage->Write( yBufferSize, pubBuffer );

	//	remember new relative position
	TpsAssert( (pStorage->GetPosition() - yAbsoluteBase) == (m_yMarker + yBufferSize), 
		"marker at wrong position" );
	m_yMarker += yBufferSize;
	
	//	remember new chunk data area size if we grew the chunk
	TpsAssert( !fNonContiguousWrite || ((m_yMarker - m_ChunkHeader.ylChunkDataPos) > m_ChunkHeader.ylChunkDataSize), "data area wrong size" );
	if ( (m_yMarker - m_ChunkHeader.ylChunkDataPos) > m_ChunkHeader.ylChunkDataSize )
		{
		YStreamLength yOldLen = m_ChunkHeader.ylChunkDataSize;
		if ( fNonContiguousWrite )
			//	the new write is a seperate data area; add its size to the earlier area(s)
			m_ChunkHeader.ylChunkDataSize += yBufferSize;
		else
			//	the write is in a single data area; its size is our end position minus our start position
			m_ChunkHeader.ylChunkDataSize = m_yMarker - m_ChunkHeader.ylChunkDataPos;
		
		m_ySize += m_ChunkHeader.ylChunkDataSize - yOldLen;
		UpdateParentSize();
		}

	//	reverse the offset to the beginning of the data
	m_yMarker -= m_ChunkHeader.ylChunkDataPos;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::Read( const uBYTE* pubBuffer, const YStreamLength yBufferSize )
//
//  Description:		read data from the chunk containing the stream pointer into the given buffer.
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::Read( YStreamLength yBufferSize, uBYTE* pubBuffer )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	m_pSelectedStorage->ReadInternal( yBufferSize, pubBuffer );
	}

void RChunkStorage::ReadInternal( const YStreamLength yBufferSize, uBYTE* pubBuffer )
	{
	TpsAssert( !IsRoot(), "The root chunk has no parent so it is not accessible for data operations" );

	//	handle offset from beginning of chunk to beginning of chunk's data
	//	handle first write; remember where data was written
	if ( kInvalidChunkPos == m_ChunkHeader.ylChunkDataPos )
		m_ChunkHeader.ylChunkDataPos = m_ySize;

	//	offset the operation from the beginning of the data
	m_yMarker += m_ChunkHeader.ylChunkDataPos;

	YStreamPosition yAbsoluteBase;			//	current file pointer relative to start of this chunk
	RStorage* pStorage;							//	storage for all chunks in this tree of chunks
	BOOLEAN	fBadSeek = FALSE;
	YStreamLength yBufferSizeUse = yBufferSize;

	GetStorageAndPos( yAbsoluteBase, pStorage );
	pStorage->SeekAbsolute( yAbsoluteBase + m_yMarker );

	if ( (m_yMarker + yBufferSize - m_ChunkHeader.ylChunkDataPos) > m_ChunkHeader.ylChunkDataSize )
		{
		yBufferSizeUse = m_ChunkHeader.ylChunkDataSize - (m_yMarker - m_ChunkHeader.ylChunkDataPos);	//	only read as much as we have
		fBadSeek = TRUE;
		}

	if (yBufferSizeUse)
		pStorage->Read( yBufferSizeUse, pubBuffer );

	//	remember new relative position
	TpsAssert( (pStorage->GetPosition() - yAbsoluteBase) == (m_yMarker + yBufferSizeUse), 
		"marker at wrong position" );
	m_yMarker = pStorage->GetPosition() - yAbsoluteBase;
	
	//	throw exception now after the sream position has been updated and the data read
	Debug( ThrowFileExceptionIfTesting( kBadSeek ) );
	if ( fBadSeek )
		ThrowFileException( kBadSeek );

	//	reverse the offset from the beginning of the data
	m_yMarker -= m_ChunkHeader.ylChunkDataPos;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetPosition( ) const
//
//  Description:		return current position of stream pointer relative to start of this chunk
//
//  Returns:			
//
//  Exceptions:		File
//
// ****************************************************************************
//
YStreamLength RChunkStorage::GetPosition( ) const
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	return m_pSelectedStorage->m_yMarker;
	}

YStreamLength RChunkStorage::GetPositionInternal( ) const
	{
	return m_yMarker;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetAbsolutePositionInStream( ) const
//
//  Description:		return current position of stream pointer relative to start of this chunk
//
//  Returns:			
//
//  Exceptions:		File
//
// ****************************************************************************
//
YStreamLength RChunkStorage::GetAbsolutePositionInStream( ) const
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	YStreamLength yPos;
	RStorage *pUnusedStorage;
	m_pSelectedStorage->GetStorageAndPos( yPos, pUnusedStorage );
	return yPos;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetHeaderSizeInStream( RArchive* pArchive )
//
//  Description:		return the size of the header at the stream ptr
//
//  Returns:			^
//
//  Exceptions:		File
//
// ****************************************************************************
//
YStreamLength RChunkStorage::GetHeaderSizeInStream( RStorage* pStorage )
	{
	TpsAssert( pStorage, "Null input storage" );

	//	pStorage must be positioned at the top of the header. Remember where we are for future reference.
	YStreamPosition yStartPos = pStorage->GetPosition();

	//	Read the magic key, platform tag and header size from the header in the given archive.
	uLONG							ulMagicKey;		
	YPlatformTag				yPlatformTag;	
	YHeaderSize					yHeaderSize;	
	pStorage->Read( sizeof( ulMagicKey ),		(PUBYTE)&ulMagicKey );
	pStorage->Read( sizeof( yPlatformTag ),	(PUBYTE)&yPlatformTag );
	pStorage->Read( sizeof( yHeaderSize ),		(PUBYTE)&yHeaderSize );
	
	//	Handle byte swapping.
	if ( yPlatformTag != kWin )
		{
		ulMagicKey = tintSwapLong( ulMagicKey );
		yHeaderSize = tintSwapWord( yHeaderSize );
		}
	
	//	Run away if were not reading a chunk header.
	if ( ulMagicKey != kMagicKey ) 
		ThrowFileException( kDataFormatInvalid );

	//	Restore the stream ptr.
	pStorage->SeekAbsolute( yStartPos );

	return yHeaderSize;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::SeekAbsolute( const YStreamLength ySeekPosition )
//
//  Description:		move stream pointer
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::SeekAbsolute( const YStreamLength ySeekPosition )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	m_pSelectedStorage->SeekAbsoluteInternal( ySeekPosition );
	}

void RChunkStorage::SeekAbsoluteInternal( const YStreamLength ySeekPosition )
	{
	YStreamPosition yAbsoluteBase;			//	current file pointer relative to start of this chunk
	RStorage* pStorage;							//	storage for all chunks in this tree of chunks

	GetStorageAndPos( yAbsoluteBase, pStorage );

	//	all public stream apis work after the header and before the index
	//	so offset the given position from the end of the header
	m_yMarker = ySeekPosition;
	YStreamPosition position = m_yMarker + yAbsoluteBase;
	
	//	don't seek after end of the chunk; note that this might be after the end of the chunk's data
	//		we allow this to support non-contiguous data writes
	if ( m_yMarker > m_ySize )
		position = kInvalidChunkPos;	//	 cause an exception

	pStorage->SeekAbsolute( position );	//	 do the seek
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::SelectParentChunk( )
//
//  Description:		set currently selected chunk to parent
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChunkStorage::SelectParentChunk( )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
	m_pSelectedStorage->WriteIndex();

	m_pSelectedStorage = dynamic_cast<RChunkStorage*>(m_pSelectedStorage->m_pParentStorage);
	TpsAssert( NULL != m_pSelectedStorage, "can't select the parent of the root" );
	
	//	move stream ptr of parent chunk to end of its data (after its children)
	//	so that data writes after the chunk's children won't overwrite the children
	YStreamPosition yNewEnd = m_pSelectedStorage->m_ySize;
	if ( kInvalidChunkPos != m_pSelectedStorage->m_ChunkHeader.ylChunkDataPos )
		yNewEnd -= m_pSelectedStorage->m_ChunkHeader.ylChunkDataPos;
	SeekAbsolute( yNewEnd );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::SeekRelative( const YStreamOffset ySeekOffset )
//
//  Description:		move stream pointer based on current position.
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::SeekRelative( const YStreamOffset ySeekOffset )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	m_pSelectedStorage->SeekRelativeInternal( ySeekOffset );
	}

void RChunkStorage::SeekRelativeInternal( const YStreamOffset ySeekOffset )
	{
	YStreamPosition yAbsoluteBase;			//	current file pointer relative to start of this chunk
	RStorage* pStorage;							//	storage for all chunks in this tree of chunks

	GetStorageAndPos( yAbsoluteBase, pStorage );

	//	all public stream apis work after the header and before the index
	//	so offset the given offset from the end of the header
	YStreamPosition position = m_yMarker + ySeekOffset;

	//	don't seek to before end of the header
#if defined _WINDOWS	// GCB 10/10/96 __int64 is a Microsoft-specific extension and shouldn't be used
	__int64 qTestOffset = m_yMarker + ySeekOffset;
#else
	double qTestOffset = m_yMarker + ySeekOffset;
#endif
	TpsAssert( ULONG_MAX > qTestOffset, "Stream position overflowed" );			//	verify the cast on the next line
	
	//	don't seek after end of the chunk; note that this might be after the end of the chunk's data
	//		we allow this to support non-contiguous data writes
	if ( qTestOffset > m_ySize )
		position = kInvalidChunkPos;	//	cause an exception

	SeekAbsoluteInternal( position );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::WriteHeader( )
//
//  Description:		write our header
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::WriteHeader( BOOLEAN fFirstWrite /*= FALSE*/ )
	{
	YStreamPosition yAbsoluteBase;			//	current file pointer relative to start of this chunk
	RStorage* pStorage;							//	storage for all chunks in this tree of chunks
	YStreamPosition ylTempChunkInfoPos;		//	temp storage for info pos which may be written out differently than its value in memory

	GetStorageAndPos( yAbsoluteBase, pStorage );

	TpsAssert( yAbsoluteBase >= m_ChunkHeader.yHeaderSize, "chunk malformed" );

	//	all normal calcs are relative to the beginning of user data
	//	but we must write the header which precedes user data so move the absolute base offset
	//	back before the user data to the beginning of the header
	yAbsoluteBase -= m_ChunkHeader.yHeaderSize;	

	//	write our header at the beginning of our chunk
	//	we need a special function to do this since the public
	//	Write method cannot write in the header area
	//	remember the stream position so it may be fixed up later
	YStreamPosition yOldPos = pStorage->GetPosition();

	pStorage->SeekAbsolute( yAbsoluteBase );
	
	//	make sure we specify the index location correctly for chunks with no children
	ylTempChunkInfoPos = m_ChunkHeader.ylChunkInfoPos;
	if ( kInvalidChunkPos == m_ChunkHeader.ylChunkInfoPos )
		m_ChunkHeader.ylChunkInfoPos = 0;

	//	record that the chunk's data has not been completely written
	if ( fFirstWrite )
			SetFlag( m_ChunkHeader.yfHeaderFlags, kWriteIncomplete );

	pStorage->Write( m_ChunkHeader.yHeaderSize, (PUBYTE)&m_ChunkHeader );
	m_ChunkHeader.ylChunkInfoPos = ylTempChunkInfoPos;

	ClearFlag( m_ChunkHeader.yfHeaderFlags, kWriteIncomplete );

	//	fix up stream position
	pStorage->SeekAbsolute( yOldPos );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::WriteIndex( RChunkStorage* pParent )
//
//  Description:		adjust our size in our parent's header and index
//								write our header and index to our parent.
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::WriteIndex( )
	{
	if ( !FlagIsSet( m_yRootFlags, kIndexWritten ) )
		{
		RCInfoList::YIterator rIter;

		if ( 0 != GetChunkListCount() )					//	if there is a child before us
			{															// then write its index
			//	if the child has already had its index written then this is unnecessary
			//	and WriteIndex() will do nothing but we must check anyway

			//	get the index of the last child in our chunk
			//	we only worry about the last child in this chunk because earlier children
			//	had their indices written when the chunk after them was added
			GetChunkListStart(  rIter  );

			YChunkInfo& yChunkBeforeUs = rIter[ GetChunkListCount() - 1 ];
			
			//	put the new header after the data and indices of the chunk before us
			yChunkBeforeUs.pChunk->WriteIndex();
			}

		//	write our children's indices first
		for( GetChunkListStart(  rIter  ); rIter != m_ChunkInfoList.End( ); ++rIter )
			(*rIter).pChunk->WriteIndex();

		DoneWriting();
		
		//	our base size is our data size plus our header size
		YStreamLength			yChunkSize = m_ySize + m_ChunkHeader.yHeaderSize;	
		YStreamPosition 		yIndexBase;				//	position in entire stream of the start of chunk's index
		YStreamPosition 		yAbsoluteBase;			//	position in entire stream of the start of this chunk
		RStorage* 				pStorage;				//	storage for all chunks in this tree of chunks

		GetStorageAndPos( yAbsoluteBase, pStorage );

		//	write the completed header; it has been modified as data was written to the chunk
		WriteHeader( );

		//	write the index after the end of the chunk's data
		//	we need to call the storage methods directly to do this since the public
		//	Write method cannot write in the index area
		//	remember the stream position so it may be fixed up later
		//	subtract the header size since it is included in both the infopos and the absolute base and we need it only once
		YStreamPosition yOldPos = pStorage->GetPosition();

		if ( kInvalidChunkPos == m_ChunkHeader.ylChunkInfoPos )
			{		
			m_ChunkHeader.ylChunkInfoPos = 0;	//	we now know that the indices should be right after the header
			yOldPos = yAbsoluteBase;				//	the header wasn't counted in the old position; add it now
			}
		
// GCB 10/11/96 Metrowerks gave "unexpected end of line" error
//	#if TPSDEBUG
	#ifdef TPSDEBUG
		YStreamPosition ySize = m_ChunkHeader.ylChunkDataSize;

		//	make sure our size is correct
		for( GetChunkListStart(  rIter  ); rIter != m_ChunkInfoList.End( ); ++rIter )
			{
			ySize += rIter[0].yChunkSize;
			}
		
		TpsAssert( (0 == GetNumberOfChunks()) || (ySize == m_ySize), "chunk wrong size" );
	#endif

	yIndexBase = yAbsoluteBase + m_ChunkHeader.ylChunkInfoPos;

	//	fill padded area with a known byte pattern
	if ( m_ChunkHeader.ylChunkAlignPad )
		{
		pStorage->SeekAbsolute( yIndexBase - m_ChunkHeader.ylChunkAlignPad );
		uLONG ulPad = kExtraFill;
		pStorage->Write( m_ChunkHeader.ylChunkAlignPad, (PUBYTE)&ulPad );
		TpsAssert( 0 == (yIndexBase % kChunkAlign), "chunk data missaligned" );
		}

	//	write our indices
	if ( 0 < GetNumberOfChunks() )				//	our index is in our parent 
															//	 and we will write an empty index for our children below
		{
		pStorage->SeekAbsolute( yIndexBase );

		for( GetChunkListStart(  rIter  ); rIter != m_ChunkInfoList.End( ); ++rIter )
			{
			pStorage->Write( sizeof(YChunkInfo), (PUBYTE)&rIter[0] );
			}
		}
	else		//	write an empty index for this chunk which has no children
		{
		YChunkInfo yEmptyIndex;

		pStorage->SeekAbsolute( yIndexBase );
		pStorage->Write( sizeof(YChunkInfo), (PUBYTE)&yEmptyIndex );
		}

	m_yMaxSize = m_ChunkHeader.ylChunkInfoPos;		//	we can't write past our index in this stream 

	SetFlag( m_yRootFlags, kIndexWritten );
	}
}

// ****************************************************************************
//
//  Function Name:	RChunkStorage::MarkSelectedChunkDeleted( )
//
//  Description:		tell the backstore that the selected chunk's storage may be reused 
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::MarkSelectedChunkDeleted( )
	{
	TpsAssert( this == GetRootBackstore( !kCreateBackstoreIfNecessary ), "Only backstored chunks may be deleted. You must access the chunk to delete thru the root of the backstore" );
	SetFlag( m_pSelectedStorage->GetIndex().yChunkFlags, kChunkDeleted );
	}

// ****************************************************************************
//
//  Function Name:	RChunkStorage::AddSubChunk( RChunkStorage* pParent, const YChunkTag yChunkTag)
//
//  Description:		insert an empty chunk and position the stream pointer so that a  
//								subsequent read/write will access the new chunk.
//							The new chunk will contain other chunks
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
YChunkId RChunkStorage::AddSubChunk( const YChunkTag yChunkTag )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	YChunkId yNewId = AddChunk( yChunkTag );
	m_pSelectedStorage->m_ChunkHeader.yfHeaderFlags |= kSubChunk;

	return yNewId;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::AddChunk( RChunkStorage* pParent, const YChunkTag yChunkTag)
//
//  Description:		insert an empty chunk and position the stream pointer so that a  
//								subsequent read/write will access the new chunk.
//
//  Returns:			Nothing;
//
//  Exceptions:		File
//
// ****************************************************************************
//
YChunkId RChunkStorage::AddChunk( const YChunkTag yChunkTag )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
//	RChunkStorage* pSavedSelection = m_pSelectedStorage;

	//	if were adding into the backstore then make sure were adding into the root not the selected chunk
	if ( this == GetRootBackstore( !kCreateBackstoreIfNecessary ) )
		m_pSelectedStorage = this;

	//	make the new chunk and insert in the chunk tree below the currently selected chunk
	//	if our index has been written then we must grow into the backstore before we can add
	//	the new chunk below us
	if ( FlagIsSet( m_pSelectedStorage->m_yRootFlags, kIndexWritten ) && ( this != GetRootBackstore( !kCreateBackstoreIfNecessary ) ) )
		m_pSelectedStorage->Grow( m_pSelectedStorage->m_ySize + kChunkMinSize );

	RChunkStorage* pParent = m_pSelectedStorage;
	m_pSelectedStorage = new RChunkStorage;
	m_pSelectedStorage->m_pParentStorage = pParent;

	//	figure out where to write the header
	YStreamLength yHeaderWritePos = 0;

	//	close off the parent chunk's data area if it has been started
	//	the root does not have a raw data area
	if ( !pParent->IsRoot() && (kInvalidChunkPos == pParent->m_ChunkHeader.ylChunkDataPos) )
		SetFlag( pParent->GetIndex().yChunkFlags, kChunkContiguousDataDone );

	//	if we are writing a chunk after another chunk at this level then 
	//	 the chunk before use needs to have its index written
	YChunkId yPriorChunksIndex = pParent->GetNumberOfChunks();
	if ( 0 != yPriorChunksIndex )		//	if there is a chunk before us
		{														// then write its index
		//	our index is in our parent
		//	we are after the last component in our parent
		RCInfoList::YIterator rIter;
		pParent->GetChunkListStart( rIter );

		YChunkInfo& yChunkBeforeUs = rIter[ yPriorChunksIndex - 1 ];
		
		//	put the new header after the data and indices of the chunk before us
		yChunkBeforeUs.pChunk->WriteIndex();
		}
	yHeaderWritePos = pParent->m_ySize + pParent->m_ChunkHeader.ylChunkAlignPad;
	
	//	we are writing the header but we haven't written the chunk padding so write it
	//	this can happen if the form of the data is chunk data chunk
	if ( pParent->m_ChunkHeader.ylChunkAlignPad )
		{
		if ( yHeaderWritePos > pParent->m_ySize )
			{
			YStreamPosition yAbsoluteBase;			//	current file pointer relative to start of this chunk
			RStorage* pStorage;							//	storage for all chunks in this tree of chunks
			GetStorageAndPos( yAbsoluteBase, pStorage );
			YStreamPosition yAbsoluteSize = pStorage->GetStorageStream()->GetSize();

			if ( yAbsoluteSize % kChunkAlign )	//	we are about to write a misaligned header; write padding to compensate
				{
				TpsAssert( yHeaderWritePos == (pParent->m_ySize + pParent->m_ChunkHeader.ylChunkAlignPad), "malformed chunk found " );
	//			pStorage->SeekAbsolute( yHeaderWritePos - pParent->m_ChunkHeader.ylChunkAlignPad );
				uLONG ulPad = kExtraFill;
				pStorage->Write( pParent->m_ChunkHeader.ylChunkAlignPad, (PUBYTE)&ulPad );
	//			TpsAssert( 0 == (yAbsoluteBase % kChunkAlign), "chunk data missaligned" );
				}
			}
		}

	m_pSelectedStorage->m_yIndexInParent = pParent->GetNumberOfChunks();				//	we will use the last index in our parent

	//	make the new index entry for this chunk and install it
	YChunkInfo yNewIndex;
	yNewIndex.yChunkTag = yChunkTag;
	yNewIndex.yChunkId = m_pSelectedStorage->m_yIndexInParent;
	yNewIndex.pChunk = m_pSelectedStorage;														//	point new index to this chunk
	yNewIndex.yChunkPos = yHeaderWritePos;
	SetFlag( yNewIndex.yChunkFlags, kDeleteChunkPtrOnClose);
	yNewIndex.yChunkSize = m_pSelectedStorage->m_ChunkHeader.yHeaderSize;			//	the new chunk just has a header for now
	pParent->m_ChunkInfoList.InsertAtEnd( yNewIndex );
	pParent->m_ChunkHeader.yciNumberOfChunks = pParent->GetNumberOfChunks();

	m_pSelectedStorage->WriteHeader();		//	reserve space for our header in the stream

//	m_pSelectedStorage = pSavedSelection;	//	restore the selected storage

	return yNewIndex.yChunkId;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::DoneWriting( )
//
//  Description:		update chunk's size to include its indices
//
//  Returns:			
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::DoneWriting( )
	{
	if ( !IsRoot() )
		{
		YChunkInfo& rIndex = GetIndex( );
		rIndex.yChunkSize = m_ySize + m_ChunkHeader.ylChunkAlignPad + m_ChunkHeader.yHeaderSize + sizeof( YChunkInfo );
		
		//	writing past the current size would now flow into the indices so 
		//		force that writing to take place in a backstored object
		m_yMaxSize = m_ySize;
		UpdateParentSize( );
		}
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetCurrentChunkTag( ) const
//
//  Description:		return platform id of the chunk that contains the stream pointer
//
//  Returns:			platform tag;
//
//  Exceptions:		File
//
// ****************************************************************************
//
YChunkTag RChunkStorage::GetCurrentChunkTag( ) const
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	RChunkStorage* pChunk = dynamic_cast<RChunkStorage*>(m_pSelectedStorage->m_pParentStorage);
	if ( NULL != pChunk )
		return m_pSelectedStorage->GetIndex().yChunkTag;
	else	
		return kRootTag;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetIndex( ) const
//
//  Description:		return our index
//
//  Returns:			
//
//  Exceptions:		File
//
// ****************************************************************************
//
YChunkInfo& RChunkStorage::GetIndex( ) const
	{
	RChunkStorage* pChunk;
	YChunkId yIndex;

	//	if this is a chunk created in the backstore then use its index list as the chunk list
#ifdef kSWITCHINDEXONCHUNKPARENTTYPE
	if ( m_pDataParentStorage == m_pParentStorage ) 
		{
// REVIEW GCB 10/10/96 fix compile error on Mac
// 1) CodeWarrior gives "constness casted away" error
// 2) if dynamic_cast fails it returns NULL - is that accounted for here?!

//	ESV 1/29/97 - If the dynamic cast fails we are attempting to get the index of the root chunk.
//	This is a logic error and will assert in a debug build.
		pChunk = dynamic_cast<RChunkStorage*>(const_cast<RChunkBackstore*>GetRootBackstore( !kCreateBackstoreIfNecessary ));
		yIndex = m_yIndexInDataParent;
		}
	else	//	else use the chunk's parent
#endif
		{
// REVIEW GCB 10/10/96 fix compile error on Mac
// 1) CodeWarrior gives "constness casted away" error
// 2) if dynamic_cast fails it returns NULL - is that accounted for here?!

//	ESV 1/29/97 - If the dynamic cast fails we are attempting to get the index of the root chunk.
//	This is a logic error and will assert in a debug build.
		pChunk = dynamic_cast<RChunkStorage*>(const_cast<RStorage*>(m_pParentStorage));
		yIndex = m_yIndexInParent;
		}

	TpsAssert( NULL != pChunk, "Invalid Chunk found" );
	RCInfoList::YIterator rIter;
	TpsAssert( kInvalidChunkIndex != yIndex, "Invalid Chunk found" );
	pChunk->GetChunkListStart(  rIter  );

	return rIter[ yIndex ];
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetCurrentChunkSize( ) const
//
//  Description:		return the size of the chunk that contains the stream pointer
//
//  Returns:			data size
//
//  Exceptions:		File
//
// ****************************************************************************
//
YStreamLength RChunkStorage::GetCurrentChunkSize( ) const
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	return m_pSelectedStorage->m_ySize;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetCurrentChunkDataSize( ) const
//
//  Description:		return the data size of the chunk that contains the stream pointer
//
//  Returns:			data size
//
//  Exceptions:		File
//
// ****************************************************************************
//
YStreamLength RChunkStorage::GetCurrentChunkDataSize( ) const
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	return m_pSelectedStorage->m_ChunkHeader.ylChunkDataSize;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::IsCurrentChunkNested( ) const
//
//  Description:		TRUE: the current chunk has a parent chunk
//
//  Returns:			
//
//  Exceptions:		File
//
// ****************************************************************************
//
BOOLEAN RChunkStorage::IsCurrentChunkNested( ) const
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	//	if we have no parent we are not nested; else we are nested
	return static_cast<BOOLEAN>(( NULL != m_pSelectedStorage->m_pParentStorage ));
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::IsCurrentChunkCurrentPlatform( ) const
//
//  Description:		TRUE: the current chunk is native to the platform of the 
//								current app instance
//
//  Returns:			
//
//  Exceptions:		File
//
// ****************************************************************************
//
BOOLEAN RChunkStorage::IsCurrentChunkCurrentPlatform( ) const
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

#ifdef _WINDOWS
	return static_cast<BOOLEAN>(GetCurrentChunkTag() == kWin);
#elif defined MAC
	return static_cast<BOOLEAN>(GetCurrentChunkTag() == kMac);
#endif
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::ReadAuxData( uBYTE* pubBuffer, const YStreamLength yBufferSize )
//
//  Description:		read data from our aux data chunk into the given buffer.
//
//  Returns:			
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::ReadAuxData( const YStreamLength yBufferSize, uBYTE* pubBuffer )
	{
	RChunkStorage* pAuxChunk = GetAuxData( );		//	get the aux data for the currently selected chunk
	pAuxChunk->SeekAbsolute( 0 );						//	we always treat the aux data as an integral unit
	pAuxChunk->Read( yBufferSize, pubBuffer );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::WriteAuxData( const uBYTE* pubBuffer, const YStreamLength yBufferSize )
//
//  Description:		write data from the given buffer into its aux data chunk.
//
//  Returns:			
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::WriteAuxData( const YStreamLength yBufferSize, const uBYTE* pubBuffer )
	{
	RChunkStorage* pAuxChunk = GetAuxData( );					//	get the aux data for the currently selected chunk
	pAuxChunk->SeekAbsolute( 0 );									//	we always treat the aux data as an integral unit
	pAuxChunk->Write( yBufferSize, pubBuffer );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetAuxData( )
//
//  Description:		return the aux data for chunk.
//
//  Returns:			the aux data for this chunk;
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChunkStorage* RChunkStorage::GetAuxData( )
	{
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
	TpsAssert( IsRoot(), "illegal access of internal tree node" );

	RChunkStorage* pAuxDataRoot = dynamic_cast<RChunkStorage*>( GetRootAuxData() );

	//	select the aux data for the currently selected chunk; make a new one if the selected chunk does not have an aux data
	if ( kInvalidChunkIndex == m_pSelectedStorage->m_ChunkHeader.yIndexInAuxData )
		{
		//	we need a new aux data chunk
		pAuxDataRoot->m_pSelectedStorage = pAuxDataRoot;		//	set selection so new chunk will use aux data as its parent
		m_pSelectedStorage->m_ChunkHeader.yIndexInAuxData = pAuxDataRoot->AddChunk( kAuxData );
		}
	else
		{
		//	return the existing aux data chunk
		RCInfoList::YIterator rIter;
		pAuxDataRoot->GetChunkListStart( rIter );

		//	select the aux data for the currently selected chunk
		pAuxDataRoot->m_pSelectedStorage = rIter[ m_pSelectedStorage->m_ChunkHeader.yIndexInAuxData ].pChunk;
		}

	return pAuxDataRoot;												//	return the root of the aux data tree for use
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetNumberOfChunks( const YChunkTag yTagType, const BOOLEAN fRecursive )
//  Function Name:	RChunkStorage::GetCurrentNumberOfChunks( const YChunkTag yTagType, const BOOLEAN fRecursive )
//
//  Description:		return:
//
//  Returns:			number of chunks at current level with given tag
//
//  Exceptions:		File
//
// ****************************************************************************
//
YChunkId RChunkStorage::GetCurrentNumberOfChunks( const YChunkTag yChunkTag, const BOOLEAN fRecursive/* = FALSE*/ )
	{
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
	TpsAssert( IsRoot(), "illegal access of internal tree node" );

	return m_pSelectedStorage->GetNumberOfChunks( yChunkTag, fRecursive );
	}

YChunkId RChunkStorage::GetNumberOfChunks( const YChunkTag yChunkTag, const BOOLEAN fRecursive/* = FALSE*/ )
	{
	YChunkId yNumChunks = 0;	

	RChunkStorage* pChunk;
	for(	pChunk = GetFirstChunkChild( );
			pChunk && ( yChunkTag != pChunk->GetIndex().yChunkTag );
			pChunk = GetNextChunk( this, fRecursive ) )
			++yNumChunks;

	return yNumChunks;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetCurrentNumberOfChunks( const BOOLEAN fRecursive )
//
//  Description:		return:
//
//  Returns:			number of chunks at current level of selected chunk or if recursive 
//								all levels below this one also
//
//  Exceptions:		File
//
// ****************************************************************************
//
YChunkId RChunkStorage::GetCurrentNumberOfChunks( const BOOLEAN fRecursive /*=FALSE*/ )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	return m_pSelectedStorage->GetNumberOfChunks( fRecursive );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetNumberOfChunks( const BOOLEAN fRecursive )
//
//  Description:		return:
//
//  Returns:			number of chunks at current level or if recursive 
//								all levels below this one also
//
//  Exceptions:		File
//
// ****************************************************************************
//
YChunkId RChunkStorage::GetNumberOfChunks( const BOOLEAN fRecursive /*=FALSE*/ )
	{
	YChunkId yNumChunks = 0;	

	if ( !fRecursive )
		yNumChunks = GetChunkListCount();
	else
		{
		RChunkStorage* pChunk;
		for(	pChunk = GetFirstChunkChild( );
				pChunk;
				pChunk = GetNextChunk( this, fRecursive ) )
				++yNumChunks;
		}

	return yNumChunks;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::SelectFirstChunk( const YChunkTag yTagType, const BOOLEAN fRecursive )
//
//  Description:		selects first chunk with given tag
//
//  Returns:			none
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::SelectFirstChunk( RChunkStorage* pSearchRoot, const YChunkTag yTagType, const BOOLEAN fRecursive /*=FALSE*/ )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
	
	//	search for the first chunk with the given tag
	RChunkStorage* pChunk;
	pChunk = m_pSelectedStorage->GetFirstChunkChild( );
	if ( pChunk && (yTagType != pChunk->GetIndex().yChunkTag) )
		pChunk = pChunk->GetNextChunk( pSearchRoot, yTagType, fRecursive );	
	
	SetSelectedChunk( pChunk );
	//
	//	Reset the marker to the beginning of the chunk
	if ( pChunk )
		pChunk->m_yMarker = 0;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetChunkListStart( )
//
//  Description:		Return a chunk list iterator positioned at the beginning of the chunk list.
//							Load the chunk list from disk if necessary.
//
//  Returns:			^
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::GetChunkListStart( RCInfoList::YIterator& rIter )
	{
	//	If the info list hasn'nt been loaded from disk then load it.
	//	Note that m_ChunkHeader.yciNumberOfChunks can be < (YChunkId)m_ChunkInfoList.Count() when were in the middle of adding a chunk.
	if ( m_ChunkHeader.yciNumberOfChunks > (YChunkId)m_ChunkInfoList.Count() )
		ReadChunkIndex( kForceUpdate );
	
	//	This condition can fail when were in the middle of adding a chunk.
//	TpsAssert ( m_ChunkHeader.yciNumberOfChunks == (YChunkId)m_ChunkInfoList.Count(), "Failed to load chunk list." );
	
	//	Return an iterator positioned at the beginning of the loaded info list.
	rIter = m_ChunkInfoList.Start();
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetChunkListCount( )
//
//  Description:		Return the number of elements in the chunk list; load the 
//							chunk's children's indices if necessary.
//
//  Returns:			^
//
//  Exceptions:		File
//
// ****************************************************************************
//
YCounter RChunkStorage::GetChunkListCount( )
	{
	//	If the info list hasn'nt been loaded from disk then load it.
	//	Note that m_ChunkHeader.yciNumberOfChunks can be < (YChunkId)m_ChunkInfoList.Count() when were in the middle of adding a chunk.
	if ( m_ChunkHeader.yciNumberOfChunks > (YChunkId)m_ChunkInfoList.Count() )
		ReadChunkIndex( kForceUpdate );
	
	//	This condition can fail when were in the middle of adding a chunk.
//	TpsAssert ( m_ChunkHeader.yciNumberOfChunks == (YChunkId)m_ChunkInfoList.Count(), "Failed to load chunk list." );
	
	return m_ChunkInfoList.Count();
	}

// ****************************************************************************
//
//  Function Name:	RChunkStorage::SelectFirstChunk( )
//
//  Description:		selects first child
//
//  Returns:			none
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::SelectFirstChunk( )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
	
	RChunkStorage* pChunk = m_pSelectedStorage->GetFirstChunkChild( );
	SetSelectedChunk( pChunk );
	//
	//	Reset the marker to the beginning of the chunk
	if ( pChunk )
		pChunk->m_yMarker = 0;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::SelectNextChunk( const YChunkId yCurrentChunk, const YChunkTag yTagType, const BOOLEAN fRecursive )
//
//  Description:		select chunk following given chunk
//
//  Returns:			none
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::SelectNextChunk( RChunkStorage* pSearchRoot, const YChunkTag yTagType, const BOOLEAN fRecursive /*=FALSE*/ )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
	
	RChunkStorage* pChunk = m_pSelectedStorage->GetNextChunk( pSearchRoot, yTagType, fRecursive );
	
	SetSelectedChunk( pChunk );
	//
	//	Reset the marker to the beginning of the chunk
	if ( pChunk )
		pChunk->m_yMarker = 0;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::SelectNextChunk( const BOOLEAN fRecursive )
//
//  Description:		select chunk following given chunk
//
//  Returns:			none
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RChunkStorage::SelectNextChunk( RChunkStorage* pSearchRoot, const BOOLEAN fRecursive /*=FALSE*/ )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
	
	RChunkStorage* pChunk = m_pSelectedStorage->GetNextChunk( pSearchRoot, fRecursive );
	
	SetSelectedChunk( pChunk );
	//
	//	Reset the marker to the beginning of the chunk
	if ( pChunk )
		pChunk->m_yMarker = 0;
	}



// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetNextChunk( const YChunkTag yChunkTag, const BOOLEAN fRecursive )
//
//  Description:		return:
//
//  Returns:			chunk at current level following given chunk
//
//  Exceptions:		File
//
// ****************************************************************************
//
RChunkStorage* RChunkStorage::GetNextChunk( RChunkStorage* pSearchRoot, const YChunkTag yChunkTag, const BOOLEAN fRecursive /*= FALSE*/ )
	{
	//	search for the first chunk with the given tag
	RChunkStorage* pChunk;
	for(	pChunk = GetNextChunk( pSearchRoot, fRecursive );
			pChunk && ( yChunkTag != pChunk->GetIndex().yChunkTag );
			pChunk = pChunk->GetNextChunk( pSearchRoot, fRecursive ) );

	return pChunk;
	}



// ****************************************************************************
//
//  Function Name:	RChunkStorage::GetNextChunk( const BOOLEAN fRecursive )
//
//  Description:		return:
//
//  Returns:			chunk at current level following given chunk
//
//  Exceptions:		File
//
// ****************************************************************************
//
RChunkStorage* RChunkStorage::GetNextChunk( RChunkStorage* pSearchRoot, const BOOLEAN fRecursive /*= FALSE*/ )
	{
	//	find the current chunk in the index
	RChunkStorage *pFound = NULL;
	RChunkStorage *pChunk = this;
	RChunkStorage* pParentChunk = NULL;
	RCInfoList::YIterator rIter;

	if (!fRecursive)
		{
		pParentChunk = dynamic_cast<RChunkStorage*>(m_pParentStorage);
		if ( NULL != pParentChunk )
			{
			TpsAssert( kInvalidChunkIndex != m_yIndexInParent, "Invalid Chunk found" );
			//	if the next chunk exists at the current level return it
			pParentChunk->GetChunkListStart( rIter );
			rIter += pChunk->m_yIndexInParent;
			if ( rIter != pParentChunk->m_ChunkInfoList.End() ) 
				{
				++rIter;
				if ( rIter != pParentChunk->m_ChunkInfoList.End() )
					pFound = rIter[0].pChunk;
				}
			}
		}
	else
		{
		//	for recursive search examine children in depth first order:
		//	go down to first child
		GetChunkListStart(  rIter  );
		if ( rIter != m_ChunkInfoList.End() )
			pFound = rIter[0].pChunk;
		
		if ( NULL == pFound )	
			{
			//	no first child; go right, go up until you can go right if necessary
			//	dont search above the root of this search
			for (	pParentChunk = dynamic_cast<RChunkStorage*>(m_pParentStorage); 
				( NULL == pFound ) && (NULL != pParentChunk) && (pSearchRoot != pParentChunk) && (NULL == pParentChunk->GetStorageStream()); 
				pParentChunk = dynamic_cast<RChunkStorage*>(pParentChunk->m_pParentStorage) )
				{
				TpsAssert( kInvalidChunkIndex != pChunk->m_yIndexInParent, "Invalid Chunk found" );
				pParentChunk->GetChunkListStart( rIter );
				rIter += pChunk->m_yIndexInParent;
				if ( rIter != pParentChunk->m_ChunkInfoList.End() ) 
					{
					++rIter;
					if ( rIter != pParentChunk->m_ChunkInfoList.End() )
						pFound = rIter[0].pChunk;
					}
				
				pChunk = pParentChunk;	// move this chunk ptr up so it is below its parent on next loop iteration
				}
			}
		}

	return pFound;
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::Start( )
//
//  Description:		return an iterator configured for a search 
//
//  Returns:			^
//
//  Exceptions:		file
//
// ****************************************************************************
//
RChunkStorage::RChunkSearcher RChunkStorage::Start( const BOOLEAN fRecursive /*= FALSE*/ )
	{
	RChunkSearcher yIter;

	//	Copy in the given search params
	yIter.m_fRecursive = fRecursive;
	
	//	Return an iterator positioned at the start of the search.
	return Start( yIter );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::Start( )
//
//  Description:		return an iterator configured for a search 
//
//  Returns:			^
//
//  Exceptions:		file
//
// ****************************************************************************
//
RChunkStorage::RChunkSearcher RChunkStorage::Start( const YChunkTag yChunkTag, const BOOLEAN fRecursive )
	{
	RChunkSearcher yIter;

	//	Copy in the given search params
	yIter.m_fUseTag = TRUE;
	yIter.m_yTag = yChunkTag;
	yIter.m_fRecursive = fRecursive;
	
	//	Return an iterator positioned at the start of the search.
	return Start( yIter );
	}


// ****************************************************************************
//
//  Function Name:	RChunkStorage::Start( )
//
//  Description:		return an iterator configured for a search 
//
//  Returns:			^
//
//  Exceptions:		file
//
// ****************************************************************************
//
RChunkStorage::RChunkSearcher RChunkStorage::Start( const YChunkTag yChunkTag, const YChunkId yChunkId, const BOOLEAN fRecursive )
	{
	RChunkSearcher yIter;

	//	Copy in the given search params
	yIter.m_fUseTag = TRUE;
	yIter.m_yTag = yChunkTag;
	yIter.m_fUseId = TRUE;
	yIter.m_yId = yChunkId;
	yIter.m_fRecursive = fRecursive;
	
	//	Return an iterator positioned at the start of the search.
	return Start( yIter );
	}

// ****************************************************************************
//
//  Function Name:	RChunkStorage::Start( )
//
//  Description:		return an iterator configured for a search 
//
//  Returns:			^
//
//  Exceptions:		file
//
// ****************************************************************************
//
RChunkStorage::RChunkSearcher RChunkStorage::Start( RChunkSearcher yIterSearchParams )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );

	//	Remember our saved selection.
	yIterSearchParams.m_pSavedSelection = m_pSelectedStorage;
	yIterSearchParams.m_pTreeRoot = this;

	//	Start a general search and narrow the results based on the given search params.
	SelectFirstChunk( );

	//	Find chunk with given search charecteristics.
	for(	; m_pSelectedStorage
			&& ( (yIterSearchParams.m_fUseTag && (GetCurrentChunkTag() != yIterSearchParams.m_yTag))
				|| (yIterSearchParams.m_fUseDataVersion && (GetCurrentChunkMinDataVersion() != yIterSearchParams.m_yDataVersion))
				|| (yIterSearchParams.m_fUseId && (GetCurrentChunkId() != yIterSearchParams.m_yId)) )
			; SelectNextChunk( yIterSearchParams.m_pTreeRoot, yIterSearchParams.m_fRecursive ) )
				NULL;

	//	Copy in the search results and return the search iterator.

	return yIterSearchParams;
	}


// ****************************************************************************
//
// Function Name:		RChunkStorage::PreserveFileDate( const BOOLEAN fPreserveFileDate )
//
// Description:		Change the file's preserve file date status.
//
// Returns:				nothing
//
// Exceptions:			none
//
// ****************************************************************************
//
void RChunkStorage::PreserveFileDate( const BOOLEAN fPreserveFileDate /* = TRUE*/ )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
//	RChunkStorage *pRoot = GetFirstChunk();

	/*pRoot->*/GetStorage( this )->PreserveFileDate( fPreserveFileDate );	
	}


// ****************************************************************************
//
// Function Name:		RChunkStorage::ShouldPreserveFileDate( )
//
// Description:		Return the file's preserve file date status.
//
// Returns:				^
//
// Exceptions:			none
//
// ****************************************************************************
//
BOOLEAN RChunkStorage::ShouldPreserveFileDate( )
	{
	TpsAssert( IsRoot(), "illegal access of internal tree node" );
	TpsAssert( m_pSelectedStorage, "no chunk selected" );
//	RChunkStorage *pRoot = GetFirstChunk();

	return /*pRoot->*/GetStorage( this )->ShouldPreserveFileDate( );	
	}


// ****************************************************************************
//
//  Function Name:	ClearFlag( )
//
//  Description:		clear the given flag from the given flag variable.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
void ClearFlag( YFlags& yFlagStorage, const YFlags yFlag )
	{
	yFlagStorage &= ~yFlag;
	}


// ****************************************************************************
//
//  Function Name:	ToggleFlag( )
//
//  Description:		toggle the given flag in the given flag variable.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
void ToggleFlag( YFlags& yFlagStorage, const YFlags yFlag )
	{
	yFlagStorage ^= yFlag;
	}


// ****************************************************************************
//
//  Function Name:	SetFlag( )
//
//  Description:		Set the given flag in the given flag variable.
//
//  Returns:			Nothing;
//
//  Exceptions:		None
//
// ****************************************************************************
//
void SetFlag( YFlags& yFlagStorage, const YFlags yFlag )
	{
	yFlagStorage |= yFlag;
	}



// ****************************************************************************
//
//  Function Name:	IsFlagSet( )
//
//  Description:		return true if the given flag is set in the given flag variable.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN FlagIsSet( YFlags yFlagStorage, const YFlags yFlag )
	{
	return static_cast<BOOLEAN>(yFlagStorage & yFlag);
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RChunkStorage::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RChunkStorage::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RChunkStorage, this );
	}

#endif	// TPSDEBUG
