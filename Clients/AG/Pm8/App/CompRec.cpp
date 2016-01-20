/*
// $Header: /PM8/App/CompRec.cpp 2     3/08/99 1:14p Rgrenfel $
//
// Component data record routines.
//
// $Log: /PM8/App/CompRec.cpp $
// 
// 2     3/08/99 1:14p Rgrenfel
// Fixed the clone method to copy over the record ID.
// 
// 17    3/05/99 6:50p Rgrenfel
// Fixed the clone method to copy over the record ID.
// 
// 16    3/01/99 11:49a Lwilson
// Added linked data support for graphic components.
// 
// 15    2/26/99 8:41p Rgrenfel
// Added support for CGM components.
// 
// 14    2/26/99 2:04p Lwilson
// Updated  CPLinkedPicture::Read() to use huge_read when reading from the
// storage.
// 
// 13    2/25/99 3:31p Lwilson
// Added code to store graphics using the graphic record implementation.
// This allows for save by reference for image/graphic components, graphic
// caching, and graphic consolidation (one copy of the image data no
// matter how many times it is used.).
// 
// 12    2/17/99 1:50p Lwilson
// Restructured the component record's serialization.  It no longer uses
// an embedded file.  Instead, it stores the component data as part of the
// record.
// 
// 11    2/11/99 10:05a Lwilson
// Updated EmbedComponent() to make sure there is a component to embed
// before continuing.
// 
// 10    2/10/99 5:01p Rgrenfel
// Added the ability to detach a component document from its record so it
// can be owned by the caller.
// 
// 9     2/09/99 3:29p Lwilson
// To fix some serialization problems, the call to EmbedComponent was
// moved from the component record to the component object.
// 
// 8     2/09/99 9:48a Lwilson
// Fixed serialization bug that was preventing the component data from
// being serialized due to io limits.  Also, did some work to match
// release() calls to getrecord() calls.
// 
// 7     2/08/99 3:05p Rgrenfel
// Called FreeDocumentContents prior to deletion.  Cleans up Grouped
// Component destruction so that they don't leak their sub-objects.
// 
// 6     2/04/99 10:09a Lwilson
// Modified serialization to always save the component data
// 
// 5     1/19/99 3:26p Johno
// Remarked out hard coded debug path in
// ComponentRecord::GetComponentView()
// 
// 4     1/11/99 2:07p Johno
// Got rid of debug exception
// 
// 3     1/04/99 5:19p Lwilson
// Removed some debugging code.
// 
// 2     12/10/98 10:59a Lwilson
// Removed byte alignment pragmas from framework dependencies, as they are
// now implemented in the individual framework includes.  Also, removed
// FrameworkIncludes.h as it is now in the precompiled header.
// 
// 1     12/08/98 5:13p Lwilson
// Support for Print Shop integration.
// 
*/

#include "stdafx.h"
ASSERTNAME

#include "pmw.h"		// Global Path Manager access.
#include "pmgdb.h"
#include "comprec.h"
#include "grafrec.h"
#include "docrec.h"
#include "memdev.h"
#include "CreateData.h"

// Framework support
#include "ApplicationGlobals.h"
#include "ComponentApplication.h"
#include "ComponentDocument.h"
#include "ComponentManager.h"
#include "ComponentTypes.h"
#include "ComponentView.h"
#include "ChunkyStorage.h"
#include "BufferStream.h"
#include "Buffer.h"

// Interfaces
#include "ImageInterface.h"
#include "GraphicInterface.h"
#include "PropertyInterface.h"

const YChunkTag	kLinkedDataID =			'LDAT';

static CMap<GRAPHIC_TYPE, GRAPHIC_TYPE, RGraphicInterface::EGraphicType, RGraphicInterface::EGraphicType> _GraphicTypeMap;

CGraphicData::CGraphicData( GRAPHIC_CREATE_STRUCT* pData )
{
	cdDataType = kGraphicData;
	cdData     = pData;
}

CComponentData::CComponentData( RComponentDocument* pData )
{
	cdDataType = kComponentData;
	cdData     = pData;
}

/*
// The constructor for a graphic record.
*/
ComponentRecord::ComponentRecord( DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where )
	: DatabaseRecord( number, type, owner, where )
	, m_pComponentDoc( NULL )
	, m_pInternalData( NULL )
	, m_pLinkedData( NULL )
{
	memset( &record, 0, sizeof(record) );

	if (_GraphicTypeMap.IsEmpty())
	{
		// Initialize graphic type map
		_GraphicTypeMap.SetAt( GRAPHIC_TYPE_CGM,			RGraphicInterface::kCGM );
		_GraphicTypeMap.SetAt( GRAPHIC_TYPE_PS_SQUARE,	RGraphicInterface::kSquare );
		_GraphicTypeMap.SetAt( GRAPHIC_TYPE_PS_ROW,		RGraphicInterface::kRow );
		_GraphicTypeMap.SetAt( GRAPHIC_TYPE_PS_COLUMN,	RGraphicInterface::kColumn );
	}
}

/*
// The destructor for a graphic record.
*/
ComponentRecord::~ComponentRecord()
{
	if (m_pComponentDoc)
	{
		m_pComponentDoc->FreeDocumentContents();
		delete m_pComponentDoc;
	}

	if (m_pLinkedData)
		m_pLinkedData->Release();
}

/*
// The creator for a graphic record.
*/
ERRORCODE ComponentRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	/* Create the new array record. */
	ComponentRecord *pCompRec = new ComponentRecord( number, type, owner, where );

	if (!pCompRec)
	{
		return ERRORCODE_Memory;
	}

	if (creation_data)
	{
		pCompRec->SetComponentData( (CCreationData *) creation_data );
	}

	pCompRec->inc_record();
	*record = pCompRec;

	return ERRORCODE_None;
}

/*
// Assign method.
*/

ERRORCODE ComponentRecord::assign( DatabaseRecordRef srecord )
{
	/* Assign the base record first. */
	ERRORCODE error = DatabaseRecord::assign( srecord );

	if (ERRORCODE_None == error)
	{
		ComponentRecord& Record = (ComponentRecord&) srecord;

		record = Record.record;
		record.reference_count = 1;

		if (Record.m_pLinkedData)
		{
			error = Record.m_pLinkedData->Clone( database, m_pLinkedData );
		}

		if (Record.m_pComponentDoc)
		{
			error = WriteInternalData( Record.m_pComponentDoc );
		}
		else if (Record.m_pInternalData)
		{
			/* Do the dup. */
			m_pInternalData = new RBufferStream();
			m_pInternalData->Copy( Record.m_pInternalData );
		}
	}

	return error;
}

/*
// Destroy method.
*/
void ComponentRecord::destroy(void)
{
	if (m_pLinkedData)
		m_pLinkedData->Destroy();

	DatabaseRecord::destroy();
}

/*
// Increment the reference count.
*/
int ComponentRecord::inc_record()
{
	return ++record.reference_count;
}

/*
// Release the record.
*/
int ComponentRecord::free_record()
{
	return --record.reference_count;
}

/*
// ReadData()
//
// Read the record.
*/
ERRORCODE ComponentRecord::ReadData( StorageDevicePtr device )
{
	ERRORCODE error = device->read_record( &record, sizeof(record) );

	if (ERRORCODE_None == error)
	{
		m_pInternalData = new RBufferStream();
		error = device->huge_read( m_pInternalData->GetBuffer( record.data_size ), record.data_size );

		// Make a Framework storage out of the internal data
		RChunkStorage storage( record.data_size, m_pInternalData->GetBuffer( record.data_size ) );

		// We need to check for linked data right away,
		// because it may be necessary before the component
		// is needed.  Note: the component could be loaded
		// here as well, but might as well load it only
		// when it is about to be used.
		if (!storage.Start( kLinkedDataID, FALSE ).End( ))
		{
			DWORD clsidLinkedData;
			storage >> clsidLinkedData;

			m_pLinkedData = CPLinkedData::CreateFromClassId( clsidLinkedData, database );
			m_pLinkedData->Read( storage );

			storage.SelectParentChunk();
		}
	}

	return error;
}

/*
// WriteData()
//
// Write the record.  Note: it is actually up to the owner of
// a compoent record to embed the data.  This has to do with
// the fact that records should not be added to the database
// in the middle of serializing other records.
*/

ERRORCODE ComponentRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error = device->write_record( &record, sizeof(record) );

	if (!m_pInternalData )
	{
		TpsAssertAlways( "No component data to write!" );
		error = ERRORCODE_Fail;
	}
	else if (ERRORCODE_None == error)
	{
		m_pInternalData->SeekAbsolute( 0 );
		error = device->huge_write( m_pInternalData->GetBuffer( record.data_size ), record.data_size );
	}

	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE ComponentRecord::SizeofData(StorageDevicePtr device)
{
	if (m_pComponentDoc)
	{
		WriteInternalData( m_pComponentDoc );
	}

	ST_MAN_SIZE size = device->size_record( sizeof(record) ) + record.data_size;

	return size;
}

ERRORCODE ComponentRecord::WriteInternalData( RComponentDocument* pComponentDoc )
{
	TpsAssert( pComponentDoc, "Invalid Argument!" );
	ERRORCODE error = ERRORCODE_None;

	try
	{
		if (m_pInternalData)
		{
			// Reset data and associated values
			record.data_size = 0;
			delete m_pInternalData;
		}

		// Open the internal data stream
		m_pInternalData = new RBufferStream();

		// Create a memory storage
		RStorage parentStorage( m_pInternalData );
		RChunkStorage storage( &parentStorage );

		try
		{
			if (m_pLinkedData)
			{
				// Create a new chunk for the linked data
				storage.AddChunk( kLinkedDataID );
				storage << m_pLinkedData->GetClsId();

				m_pLinkedData->Write( storage );
				storage.SelectParentChunk( );
			}

			// Create a new chunk for the component
			storage.AddChunk( kComponent );

			// Serialize the component type so it can be recreated on loading
			YComponentType strComponentType = pComponentDoc->GetComponentType( );
			storage << strComponentType;

			// Write the component to the storage
			pComponentDoc->Write( storage, kSaving );

			// Get back to the top level
			storage.SelectParentChunk( );
		}
		catch (YException)
		{
			error = ERRORCODE_Fail;
		}

		// Get the size and data of the storage
		storage.GetCurrentChunkBuffer( record.data_size );

		/* We have been modified. */
		modified();
	}
	catch (...)
	{
	}

	return error;
}

/*
// Get the component.
*/
RComponentView* ComponentRecord::GetComponentView()
{
	if (!m_pComponentDoc)
	{
		TpsAssert( m_pInternalData, "Uninitialized Component Record!" );
		m_pInternalData->SeekAbsolute( 0 );

		// Make a Framework storage out of the internal data
		RChunkStorage storage( record.data_size, m_pInternalData->GetBuffer( record.data_size ) );

		if (!storage.Start( kComponent, FALSE ).End( ))
		{
			// Create the component from storage
			YComponentType strComponentType;
			storage >> strComponentType;

			m_pComponentDoc = ::GetComponentManager().CreateNewComponent(
				strComponentType, NULL, RComponentAttributes(), TRUE );

			// Read the component from storage
			m_pComponentDoc->Read( storage, kLoading );
			storage.SelectParentChunk();
		}

		// Now that we have the component data, 
		// we don't need to keep the internal 
		// representation around;
		delete m_pInternalData;
		m_pInternalData = NULL;

		if (m_pLinkedData)
			m_pLinkedData->Attach( m_pComponentDoc );
	}

	if (m_pComponentDoc)
	{
		return (RComponentView *) m_pComponentDoc->GetActiveView();
	}

	return NULL;
}


/*
// Set the component info from the component passed in.
*/
ERRORCODE ComponentRecord::SetComponentData( CCreationData* pData )
{
	ERRORCODE error = ERRORCODE_None;
	TpsAssert( !m_pComponentDoc && !m_pInternalData, "Record already contains a component!" );

	switch (pData->cdDataType)
	{
		case kComponentData:
			m_pComponentDoc = ((CComponentData *) pData)->cdData;
			break;

		case kGraphicData:
		{
			GRAPHIC_CREATE_STRUCT* pGraphicData = ((CGraphicData *) pData)->cdData;

			// Create a new component
			//
			RRealVectorRect rcBounds( 0, 0, kSystemDPI, kSystemDPI ); // The actual size doesn't really matter
			GRAPHIC_TYPE graphic_type = type_of_graphic( pGraphicData->m_csFileName, pGraphicData->pSourceDevice );
			YComponentType type = ComponentTypeFromGraphicType( graphic_type );

			m_pComponentDoc = ::GetComponentManager().CreateNewComponent(
				type, NULL, NULL, RComponentAttributes(), rcBounds, FALSE );

			if (!m_pComponentDoc)
				return ERRORCODE_Fail;

			m_pLinkedData = CPLinkedData::CreateFromClassId( CPLinkedPicture::IDCLS, database );

			if (m_pLinkedData && m_pLinkedData->Attach( m_pComponentDoc ) != ERRORCODE_None)
			{
				// Failed to attach to component, so 
				// release pointer, and set it to null.
				m_pLinkedData->Release();
				m_pLinkedData = NULL;
			}

			if (m_pLinkedData)
				error = ((CPLinkedPicture *) m_pLinkedData)->SetGraphicInfo( pGraphicData, m_pComponentDoc );
			else
				error = EmbedPicture( m_pComponentDoc, pGraphicData, graphic_type );

			break;
		}

		default:
			TpsAssertAlways( "Invalid creation data!" );
			break;
	}

	return error;
}

/*	method to return YComponentType from GRAPHIC_TYPE
*/
YComponentType ComponentRecord::ComponentTypeFromGraphicType( GRAPHIC_TYPE graphic_type )
{
	switch (graphic_type)
	{
	case GRAPHIC_TYPE_PS_SQUARE:	return kSquareGraphicComponent;
	case GRAPHIC_TYPE_PS_ROW:		return kRowGraphicComponent;
	case GRAPHIC_TYPE_PS_COLUMN:	return kColumnGraphicComponent;
	case GRAPHIC_TYPE_CGM:			return kCGMGraphicComponent;
	}

	return kImageComponent; 
}

ERRORCODE ComponentRecord::EmbedPicture( RComponentDocument* pComponentDoc, GRAPHIC_CREATE_STRUCT *pGraphicData, GRAPHIC_TYPE type )
{
	ERRORCODE error = ERRORCODE_Fail;
	ReadOnlyFile file( pGraphicData->m_csFileName );

	ST_DEV_POSITION lSize;
	file.length( &lSize );	

	RBufferStream stream;
	file.huge_read( stream.GetBuffer( lSize ), lSize );
	RStorage storage( &stream );

	RComponentView* pView = (RComponentView *) pComponentDoc->GetActiveView();

	if (pView)
	{
		RImageInterface * pImageInterface = dynamic_cast<RImageInterface*>(pView->GetInterface(kImageInterfaceId));

		if (pImageInterface)
		{
			YException hException;

			if (!pImageInterface->Load( storage, lSize, hException ))
			{
				// Try from the file instead
				CString strFixedPath = GetGlobalPathManager()->LocatePath( pGraphicData->m_csFileName );
				pImageInterface->Load( strFixedPath, hException );
			}

			if (pImageInterface->GetImage())
				error = ERRORCODE_None;

			delete pImageInterface;
		}
		else
		{
			RGraphicInterface* pGraphicInterface = dynamic_cast<RGraphicInterface*>(pView->GetInterface(kGraphicInterfaceId));

			if (pGraphicInterface)
			{
				if (type == GRAPHIC_TYPE_CGM)
				{
					// CGM data reads the raw bits.
					pGraphicInterface->Load( storage, lSize, RGraphicInterface::kCGM );
				}
				else
				{
					TpsAssert( type >= GRAPHIC_TYPE_PS_SQUARE && type <= GRAPHIC_TYPE_PS_COLUMN, "Graphic type not supported!" );
					RGraphicInterface::EGraphicType graphicType;
					_GraphicTypeMap.Lookup( type, graphicType );

					pGraphicInterface->LoadFromPretzelCollection( storage, graphicType ) ;
				}

				delete pGraphicInterface;
				error = ERRORCODE_None;
			}
		}
	}

	return error;
}

// Extracts the component document from the object's control.  Used for
// Grouped Component Object dialogs when the "More Graphics" button is processed.
RComponentDocument* ComponentRecord::DetachComponentDocument()
{
	// Get the attached document.
	RComponentDocument *pDetachedDoc = m_pComponentDoc;

	if (m_pLinkedData)
	{
		// Embed the data
		UnimplementedCode();
	}

	// Remove our interest in the document.
	m_pComponentDoc = NULL;

	// Give the previously attached document to the caller to deal with.
	return pDetachedDoc;
}


/*
// The constructor for linked data
*/
CPLinkedData::CPLinkedData()
	: m_nRefCount( 0 )
{
}

/*
// The destructor for linked data
*/
CPLinkedData::~CPLinkedData()
{
}

uLONG	CPLinkedData::AddRef( void )
{
	m_nRefCount++;

	return m_nRefCount;
}

uLONG	CPLinkedData::Release( void )
{
	uLONG ulResult = --m_nRefCount;

	if (0 == ulResult)
		delete this;

	return ulResult;
}

CPLinkedData* CPLinkedData::CreateFromClassId( DWORD clsid, DatabasePtr database )
{
	CPLinkedData* pLinkedData = NULL;

	if (clsid == CPLinkedPicture::IDCLS)
	{
		pLinkedData = new CPLinkedPicture( database );
	}

	if (pLinkedData)
		pLinkedData->AddRef();

	return pLinkedData;
}



/*
// The constructor for a linked picture
*/
CPLinkedPicture::CPLinkedPicture( DatabasePtr database )
	: m_nRecordNumber( 0 )
	, m_pRecordDatabase( database )
{
	TpsAssert( database, "Invalid arguement!" );
}

/*
// The destructor for a linked picture
*/
CPLinkedPicture::~CPLinkedPicture()
{
}

BOOLEAN CPLinkedPicture::QueryInterface( YInterfaceId iid, void ** ppvObject )
{
	if (kUnknownInterfaceId == iid)
		*ppvObject = (IUnknownInterface *) this;
	else if (kBufferInterfaceId == iid)
		*ppvObject = (IBufferInterface *) this;
	else
		*ppvObject = NULL;

	if (*ppvObject)
	{
		((IUnknownInterface *) *ppvObject)->AddRef();
	}

	return (*ppvObject != NULL);
}

ERRORCODE CPLinkedPicture::Attach( RComponentDocument* pComponentDoc )
{
	ERRORCODE error = ERRORCODE_Fail;
	RComponentView* pComponentView = NULL;
	
	if (pComponentDoc)
		pComponentView = (RComponentView *) pComponentDoc->GetActiveView();

	if (!pComponentView)
		return ERRORCODE_BadParameter;

	RImageInterface* pImageInterface = (RImageInterface *) pComponentView->GetInterface( kImageInterfaceId );

	if (pImageInterface)
	{
		if (m_nRecordNumber)
		{
			pImageInterface->SetImage( (IBufferInterface*) this, TRUE, TRUE );
		}

		delete pImageInterface;
		return ERRORCODE_None;
	}

	RGraphicInterface* pGraphicInterface = (RGraphicInterface *) pComponentView->GetInterface( kGraphicInterfaceId );

	if (pGraphicInterface)
	{
		if (m_nRecordNumber)
		{
			GraphicPtr pGraphic = (GraphicPtr) m_pRecordDatabase->get_record( m_nRecordNumber, &error, RECORD_TYPE_Graphic );
			RGraphicInterface::EGraphicType graphicType;
			_GraphicTypeMap.Lookup( (GRAPHIC_TYPE) pGraphic->record.type, graphicType );

			pGraphicInterface->Load( (IBufferInterface*) this, graphicType, TRUE );
			pGraphic->release();
		}

		delete pGraphicInterface;
		return ERRORCODE_None;
	}

	return error;
}

/*
// Clone method.
*/

ERRORCODE CPLinkedPicture::Clone( DatabasePtr database, CPLinkedData* &pLinkedData )
{
	TpsAssert( database, "Invalid arguement!" );
	TpsAssert( m_nRecordNumber, "Object has not been initialized!" );
	pLinkedData = NULL;

	CPLinkedPicture* pPictureData = (CPLinkedPicture *) CPLinkedData::CreateFromClassId( IDCLS, database );
	if (!pPictureData) return ERRORCODE_Fail;

	ERRORCODE error = ERRORCODE_None;
	PMGDatabasePtr db = (PMGDatabasePtr) database;

	if (m_pRecordDatabase == database)
	{
		/* Increment the reference for our linked data. */
		db->inc_graphic_record( m_nRecordNumber );
		// Copy over the record data since we should match.
		pPictureData->m_pRecordDatabase = m_pRecordDatabase;
		pPictureData->m_nRecordNumber = m_nRecordNumber;
	}
	else
	{
		DocumentPtr pDocument = (DocumentPtr) database->get_record( PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document );
		GraphicPtr  pGraphic  = (GraphicPtr)  m_pRecordDatabase->get_record( m_nRecordNumber, &error, RECORD_TYPE_Graphic );

		try
		{
			pPictureData->m_nRecordNumber = pDocument->find_graphic( pGraphic->m_csFileName );

			if (pPictureData->m_nRecordNumber)
			{
				/* Found it. Just bump the record ref count. */
				db->inc_graphic_record( pPictureData->m_nRecordNumber );
			}
			else
			{
				/*
				// Duplicate the record.
				// Extreme care must be taken to maintain the correct database at all times.
				*/
				GraphicPtr pNewGraphic = (GraphicPtr) pGraphic->duplicate( database, &error );

				if (pNewGraphic)
				{
					pPictureData->m_nRecordNumber = pNewGraphic->Id();
					pNewGraphic->release();

					/* Insert the graphic number into the document graphic array. */
					pDocument->add_graphic( pPictureData->m_nRecordNumber );
				}
			}
		}
		catch (...)
		{
			pPictureData->Release();
			pPictureData = NULL;

			error = ERRORCODE_Fail;
		}
	
		if (pGraphic)
			pGraphic->release();

		if (pDocument)
			pDocument->release();
	}

	pLinkedData = pPictureData;

	return error;
}

/*
// Destroy method.
*/
void CPLinkedPicture::Destroy(void)
{
	if (m_nRecordNumber)
	{
		((PMGDatabasePtr) m_pRecordDatabase)->free_graphic_record( m_nRecordNumber );
	}
}

/*
// Gets the object's class id.
*/
DWORD CPLinkedPicture::GetClsId()
{
	return IDCLS;
}

ERRORCODE CPLinkedPicture::Read( RStorage& ar )
{
	ar >> m_nRecordNumber;

	return ERRORCODE_None;
}

ERRORCODE CPLinkedPicture::SetGraphicInfo( GRAPHIC_CREATE_STRUCT* pGCS, RComponentDocument* pComponentDoc  )
{
	PMGDatabasePtr db = (PMGDatabasePtr) m_pRecordDatabase;
	m_nRecordNumber = db->new_graphic_record( pGCS );

	if (!m_nRecordNumber)
		return ERRORCODE_Fail;

	return Attach( pComponentDoc );
}

ERRORCODE CPLinkedPicture::Write( RStorage& ar )
{
	ar << m_nRecordNumber;

	return ERRORCODE_None;
}

/*
// Reads the data into the specified buffer.
*/
HRESULT CPLinkedPicture::Read( RBuffer& buffer )
{
	HRESULT hResult = S_FALSE;

	ERRORCODE error;
	GraphicPtr pGraphic = (GraphicPtr) m_pRecordDatabase->get_record( m_nRecordNumber, &error, RECORD_TYPE_Graphic );

	if (ERRORCODE_None == error)
	{
		ReadOnlyFile File;

		if (pGraphic->prep_storage_file( &File, FALSE ) == ERRORCODE_None)
		{
			ST_DEV_POSITION length = 0; //j
			File.length( &length );
//j			TpsAssert( length, "Corrupt data!" );
			buffer.Resize( length );
			if (length > 0) //j
				File.huge_read( buffer.GetBuffer(), length );
			hResult = S_OK;
		}

		pGraphic->release();
	}

	return hResult;
}

/*
// Reads the data from the specified buffer.
*/
HRESULT CPLinkedPicture::Write( RBuffer& buffer )
{
	return E_NOTIMPL;
}

HRESULT CPLinkedPicture::GetFileName( RMBCString& strFileName )
{
	ERRORCODE error;
	GraphicPtr pGraphic = (GraphicPtr) m_pRecordDatabase->get_record( m_nRecordNumber, &error, RECORD_TYPE_Graphic );
	strFileName.Empty();

	if (pGraphic)
	{
		strFileName = pGraphic->m_csFileName;
		pGraphic->release();
	}


	return S_OK;
}

