/*
// $Header: /PM8/App/CompRec.h 1     3/03/99 6:04p Gbeddow $
//
// Component record definitions.
//
// $Log: /PM8/App/CompRec.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 5     2/25/99 3:31p Lwilson
// Added code to store graphics using the graphic record implementation.
// This allows for save by reference for image/graphic components, graphic
// caching, and graphic consolidation (one copy of the image data no
// matter how many times it is used.).
// 
// 4     2/17/99 1:50p Lwilson
// Restructured the component record's serialization.  It no longer uses
// an embedded file.  Instead, it stores the component data as part of the
// record.
// 
// 3     2/10/99 5:04p Rgrenfel
// Added the ability to detach a component document from its record so it
// can be owned by the caller.
// 
// 2     2/09/99 3:29p Lwilson
// To fix some serialization problems, the call to EmbedComponent was
// moved from the component record to the component object.
// 
// 1     12/08/98 5:13p Lwilson
// Support for Print Shop integration.
// 
*/

#ifndef __COMPREC_H__
#define __COMPREC_H__

// Interface support
#include "DataInterface.h"

class RComponentDocument;
class RComponentView;
class RBufferStream;
class RChunkStorage;
class RStorage;

class CCreationData;
class CPLinkedData;

/*
// The graphic data record.
*/

class ComponentRecord : public DatabaseRecord
{
protected:

	// The constructor for this record is protected. Use the create method.
	ComponentRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

	// Assign method.
	virtual ERRORCODE			assign(DatabaseRecordRef record);

public:

	// The destructor for the record.
	~ComponentRecord();

	static ERRORCODE			create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);
	virtual VOID				destroy(VOID);
	
	virtual int					inc_record();
	virtual int					free_record();

	/***************************************************/
	/* Methods used exclusively by the StorageManager. */
	/***************************************************/

	virtual ERRORCODE			ReadData( StorageDevicePtr device );
	virtual ERRORCODE			WriteData( StorageDevicePtr device );
	virtual ST_MAN_SIZE		SizeofData( StorageDevicePtr device );

	/***************************************************/
	/* Component related functions.                    */
	/***************************************************/

	virtual RComponentView*	GetComponentView();
	virtual ERRORCODE			SetComponentData( CCreationData* pData );

	// Extracts the component document from the object's control.  Used for
	// Grouped Component Object dialogs when the "More Graphics" button is processed.
	RComponentDocument*		DetachComponentDocument();
	RComponentDocument*		GetComponentDocument() { return m_pComponentDoc; }

protected:

	ERRORCODE					WriteInternalData( RComponentDocument* pComponentDoc );

	YComponentType				ComponentTypeFromGraphicType( GRAPHIC_TYPE graphic_type );
	ERRORCODE					EmbedPicture( RComponentDocument* pComponentDoc, GRAPHIC_CREATE_STRUCT *pGraphicData, GRAPHIC_TYPE type );

private:

	RComponentDocument*		m_pComponentDoc;
	RBufferStream*				m_pInternalData;
	CPLinkedData*				m_pLinkedData;

	struct
	{
		USHORT				reference_count;	/* How many references there are */
		DWORD					data_size;			/* Size of the embedded data */

	} record;

};

const int kLinkedPictureId = 'LPID';

class CPLinkedData : public IBufferInterface
{
public:

									CPLinkedData();
	virtual						~CPLinkedData();

	virtual ERRORCODE			Attach( RComponentDocument* pComponentDoc ) = 0;
	virtual ERRORCODE			Clone( DatabasePtr database, CPLinkedData* &pLinkedData ) = 0;
	virtual VOID				Destroy( VOID ) = 0;

	virtual DWORD				GetClsId() = 0;
	virtual ERRORCODE			Read( RStorage& ar ) = 0;
	virtual ERRORCODE			Write( RStorage& ar ) = 0;

public:

	virtual uLONG				AddRef( void ); 
	virtual uLONG				Release( void ); 

	static CPLinkedData*		CreateFromClassId( DWORD clsid, DatabasePtr database );

protected:

	uLONG							m_nRefCount;
};

class CPLinkedPicture : public CPLinkedData
{
public:
									CPLinkedPicture( DatabasePtr database );
									~CPLinkedPicture( );

	ERRORCODE					SetGraphicInfo( GRAPHIC_CREATE_STRUCT* pGCS, RComponentDocument* pComponentDoc );

// Dialog Data
	enum							{ IDCLS = kLinkedPictureId };

public:
									// IUnknownInterface
	virtual BOOLEAN			QueryInterface( YInterfaceId, void ** ppvObject );

									// CLinkedData
	virtual ERRORCODE			Attach( RComponentDocument* pComponentDoc );
	virtual ERRORCODE			Clone( DatabasePtr database, CPLinkedData* &pLinkedData );
	virtual VOID				Destroy( VOID );

	virtual DWORD				GetClsId();
	virtual ERRORCODE			Read( RStorage& ar );
	virtual ERRORCODE			Write( RStorage& ar );

									// IBufferInterface
	virtual HRESULT			Read( RBuffer& buffer );
	virtual HRESULT			Write( RBuffer& buffer );
	virtual HRESULT			GetFileName( RMBCString& strFileName );
	
private:

	DB_RECORD_NUMBER			m_nRecordNumber;
	DatabasePtr					m_pRecordDatabase;
};

#endif	// __COMPREC_H__
