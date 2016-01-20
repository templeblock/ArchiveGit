/*	
	$Header: /PM8/App/BaseDataSource.cpp 1     3/03/99 6:03p Gbeddow $

	Base class to handle delayed render for Copy Paste and drag drop operations.

	Copyright 1998 The Learning Company, Inc., all rights reserved

	Revision History:

	$Log: /PM8/App/BaseDataSource.cpp $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 2     1/25/99 3:38p Rgrenfell
// Added support for object descriptors in Drag/Drop
// 
// 1     1/07/99 5:07p Rgrenfel
// New files to allow Drag out of PM
*/


#include "stdafx.h"
#include "afxadv.h"	
ASSERTNAME

#include "BaseDataSource.h"
#include "Pmw.h"
#include "PmwDoc.h"
#include "Clip.h"

// #include "ComponentSlab.h"

// Renaissance includes
// #include "bufferstream.h"
// #include "ChunkyStorage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


/*	CPBaseDataSource class constructor.
*/
CPBaseDataSource::CPBaseDataSource()
{
}

/*	CPBaseDataSource class destructor.
*/
CPBaseDataSource::~CPBaseDataSource()
{

}

#if 0
/*	CPBaseDataSource method that reads the serialized data from the
			memory file specified by <p hData.

	@rdesc	The list of objects serialized out of the memory file.

	@devnote The list returned has been dynamically allocated and it is the
			 responsibility of the caller to free its memory.
*/
CFrogOleDataList * CPBaseDataSource::DeSerialize(
	HANDLE hData )	// @parm A handle to a memory file containing the data.
{
	// associate global memory handle with shared memory file
	CSharedFile mf;
	mf.SetHandle(hData);

	CFrogOleDataList *pList = NULL;
	TRY
	{ 
		// read from shared memory file into object list
		CArchive ar (&mf, CArchive::load);
		pList = new CFrogOleDataList;
		pList->Serialize (ar);
		// disconnect archive from shared memory file
		ar.Close();
	}
	CATCH(CArchiveException, archExcept)
	{
		CBExceptionReporter::ArchiveException(archExcept);
		delete pList;
		pList = NULL;
	}
	END_CATCH
	// close shared memory file
	mf.Detach();
	
	return pList;
}


/*	CPBaseDataSource method that serializes the list of objects into
			a memory file which is returned.

	@rdesc	A handle to the memory file containing the rendered list of objects.
*/
HANDLE CPBaseDataSource::Serialize(
	CFrogOleDataList* pList )	// @parm The list of objects to serialize.
{
	// serialize list of layout objects to shared memory file
	CSharedFile mf (GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);

	TRY
	{ 
		CArchive ar (&mf, CArchive::store);
		pList->Serialize (ar);
		ar.Close();
	}
	CATCH(CArchiveException, archExcept)
	{
		CBExceptionReporter::ArchiveException(archExcept);
		return NULL;
	}
	END_CATCH

	HGLOBAL hGlobal = mf.Detach();
	ASSERT (hGlobal);

	// return global handle to caller
	return  hGlobal;
}
#endif


/*	CPBaseDataSource method that takes the list of objects and builds an
	object descriptor for the resulting transferred object and places it
	in the global handle phGlobal.

	@rdesc	TRUE on success.  FALSE if a problem occurs.
*/
BOOL CPBaseDataSource::RenderObjectDescriptor(
	HGLOBAL* phGlobal		// A pointer to a handle to be given
								// the address of the new metafile.
	)
{
	HGLOBAL hObjectDescriptor;

	ASSERT( m_pDoc != NULL );
	if (m_pDoc != NULL)
	{
		ASSERT(m_pDoc->object_list()->first_object() != NULL);

		// Create a selection list from the document.
		CPtrList *plistSelected = CreateSelectedList();

		// We now have our object. Setup the metafile.
		hObjectDescriptor = CreateObjectDescriptorFromObjects(plistSelected);

		// Release the selection list created.
		ReleaseSelectedList(plistSelected);
	}

	*phGlobal = hObjectDescriptor;

	return (hObjectDescriptor != NULL);
}	

/*	CPBaseDataSource method that renders the list of objects
			into a metafile to be provided to the application in the global
			handle phGlobal.

	@rdesc	TRUE on success.  FALSE if a problem occurs.
*/
BOOL CPBaseDataSource::RenderMetafile(
	HGLOBAL* phGlobal		// A pointer to a handle to be given
								// the address of the new metafile.
	)
{
	HGLOBAL hMetaFilePict = NULL;

	ASSERT( m_pDoc != NULL );
	if (m_pDoc != NULL)
	{
		ASSERT(m_pDoc->object_list()->first_object() != NULL);

		// Create a selection list from the document.
		CPtrList *plistSelected = CreateSelectedList();

		// We now have our object. Setup the metafile.
		hMetaFilePict = CreateMetafilePictFromObjects(plistSelected);

		// Release the selection list created.
		ReleaseSelectedList(plistSelected);
	}

	*phGlobal = hMetaFilePict;

	return (hMetaFilePict != NULL);
}


/*	CPBaseDataSource method that renders the selected objects
	into an enhanced metafile to be provided to the application via the
	global handle phGlobal provided.

	Returns : TRUE on success.  FALSE if a problem occurs.
*/
BOOL CPBaseDataSource::RenderEnhMetafile(
	HGLOBAL* phGlobal		// A pointer to a handle to be given
								// the address of the new metafile.
	)
{
	HGLOBAL hMetaFilePict = NULL;

	ASSERT( m_pDoc != NULL );
	if (m_pDoc != NULL)
	{
		ASSERT(m_pDoc->object_list()->first_object() != NULL);

		// Create a selection list from the document.
		CPtrList *plistSelected = CreateSelectedList();

		// We now have our object. Setup the metafile.
		hMetaFilePict = CreateEnhMetafilePictFromObjects(plistSelected);

		// Release the selection list created.
		ReleaseSelectedList(plistSelected);
	}

	*phGlobal = hMetaFilePict;

	return (hMetaFilePict != NULL);
}

/*	CPBaseDataSource method that renders the selected objects
	into a bitmap handle to be provided to the application via the
	global handle phGlobal provided.

	Returns : TRUE on success.  FALSE if a problem occurs.
*/
BOOL CPBaseDataSource::RenderBitmap(
	HGLOBAL* phGlobal		// A pointer to a handle to be given
								// the address of the new metafile.
	)
{
	HGLOBAL hBitmap = NULL;

	ASSERT( m_pDoc != NULL );
	if (m_pDoc != NULL)
	{
		ASSERT(m_pDoc->object_list()->first_object() != NULL);

		// Create a selection list from the document.
		CPtrList *plistSelected = CreateSelectedList();

		// We now have our object. Setup the Bitmap.
		hBitmap = RenderObjectsAsBitmap(plistSelected);

		// Release the selection list created.
		ReleaseSelectedList(plistSelected);
	}

	*phGlobal = hBitmap;

	return (hBitmap != NULL);
}


/*	CPBaseDataSource method that renders the selected objects
	into a DIB handle to be provided to the application via the
	global handle phGlobal provided.

	Returns : TRUE on success.  FALSE if a problem occurs.
*/
BOOL CPBaseDataSource::RenderDIB(
	HGLOBAL* phGlobal		// A pointer to a handle to be given
								// the address of the new metafile.
	)
{
	HGLOBAL hBitmap = NULL;

	ASSERT( m_pDoc != NULL );
	if (m_pDoc != NULL)
	{
		ASSERT(m_pDoc->object_list()->first_object() != NULL);

		// Create a selection list from the document.
		CPtrList *plistSelected = CreateSelectedList();

		// We now have our object. Setup the DIB.
		hBitmap = RenderObjectsAsDIB(plistSelected);

		// Release the selection list created.
		ReleaseSelectedList(plistSelected);
	}

	*phGlobal = hBitmap;

	return (hBitmap != NULL);
}


/*	CPBaseDataSource method that renders the provided list into
			RTF data placed into a memory handled to be placed in phGlobal.

	@rdesc	TRUE on success.  FALSE on failure.
*/
BOOL CPBaseDataSource::RenderRTF(
	HGLOBAL* phGlobal //,				// @parm A pointer to a handle to be given
									//		 the address of the new RTF file.
//	CFrogOleDataList* pObjList )	// @parm The list of objects to render.
	)
{
#if 0
	ASSERT( pObjList );
	if( !pObjList )
		return FALSE;

	// Global alloc enough memory for the text
	HGLOBAL 	hGlobal = NULL;
	uBYTE		*pGlobalData	= NULL;
	YDataLength	TotaldataLength	= 0;

	for (POSITION pos = pObjList->GetHeadPosition(); pos != NULL; )
	{
		CFrogTextCell *pObj = dynamic_cast<CFrogTextCell *>(pObjList->GetNext(pos));
		if (pObj == NULL)
			continue;
		RTextDocument *pComponentDoc = dynamic_cast<RTextDocument *>(pObj->GetComponentDocument());
		
		if (pComponentDoc == NULL)
			continue;

		uBYTE	*pData			= NULL;
		YDataLength	dataLength	= 0;

		try
		{
			RMemoryDataTransfer	transfer;
			pComponentDoc->Copy( kRTFFormat, transfer );
			transfer.GetData( kRTFFormat, pData, dataLength );
		}
		catch( ... )
		{
			delete [] pData;
			throw;
		}

		// Create a new block & copy old one
		if (pGlobalData != NULL)
		{
			HGLOBAL hTempGlobal = ::GlobalAlloc(GHND, TotaldataLength + dataLength);
			if (!hTempGlobal) 
				return FALSE;
			uBYTE	*pTempData = (uBYTE*)GlobalLock(hTempGlobal);

			// Copy the old data								
			memcpy( pTempData, pGlobalData, TotaldataLength );

			// Copy the new data								
			memcpy( pTempData + TotaldataLength - 1, pData, dataLength );

			GlobalUnlock( hGlobal );
			GlobalFree( hGlobal );
			hGlobal = hTempGlobal;

			// Update our total
			TotaldataLength += dataLength;			
		}
		else
		{
		  	// Global alloc enough memory for the text
			hGlobal = ::GlobalAlloc(GHND, dataLength);
			if (!hGlobal) 
				return FALSE;			
			pGlobalData = (uBYTE*)GlobalLock(hGlobal);
			// Copy the data								
			memcpy( pGlobalData, pData, dataLength );
			TotaldataLength = dataLength;			
		}	  
	}	// pos

  	GlobalUnlock( hGlobal );
	*phGlobal = hGlobal;

#endif	
	return TRUE;
}


/*	CPBaseDataSource method that renders the provided list into
			pure text.

	@rdesc	TRUE on success.  FALSE on failure.
*/
BOOL CPBaseDataSource::RenderText(
	HGLOBAL* phGlobal //,				// @parm A pointer to be given the address
									//		 of the memory handle with the text.
//	CFrogOleDataList* pObjList )	// @parm The list of objects to render.
	)
{
#if 0
	ASSERT( pObjList );
	if( !pObjList )
		return FALSE;

	// Global alloc enough memory for the text
	HGLOBAL 	hGlobal = NULL;
	uBYTE		*pGlobalData	= NULL;
	YDataLength	TotaldataLength	= 0;

	for (POSITION pos = pObjList->GetHeadPosition(); pos != NULL; )
	{
		CFrogTextCell *pObj = dynamic_cast<CFrogTextCell *>(pObjList->GetNext(pos));
		if (pObj == NULL)
			continue;
		RTextDocument *pComponentDoc = dynamic_cast<RTextDocument *>(pObj->GetComponentDocument());
		
		if (pComponentDoc == NULL)
			continue;

		uBYTE	*pData			= NULL;
		YDataLength	dataLength	= 0;

		try
		{
			RMemoryDataTransfer	transfer;
			pComponentDoc->Copy( kTextFormat, transfer );
			transfer.GetData( kTextFormat, pData, dataLength );
		}
		catch( ... )
		{
			delete [] pData;
			throw;
		}

		// Create a new block & copy old one
		if (pGlobalData != NULL)
		{
			HGLOBAL hTempGlobal = ::GlobalAlloc(GHND, TotaldataLength + dataLength);
			if (!hTempGlobal) 
				return FALSE;
			uBYTE	*pTempData = (uBYTE*)GlobalLock(hTempGlobal);

			// Copy the old data								
			memcpy( pTempData, pGlobalData, TotaldataLength );

			// Copy the new data								
			memcpy( pTempData + TotaldataLength - 1, pData, dataLength );

			GlobalUnlock( hGlobal );
			GlobalFree( hGlobal );
			hGlobal = hTempGlobal;

			// Update our total
			TotaldataLength += dataLength;			
		}
		else
		{
		  	// Global alloc enough memory for the text
			hGlobal = ::GlobalAlloc(GHND, dataLength);
			if (!hGlobal) 
				return FALSE;			
			pGlobalData = (uBYTE*)GlobalLock(hGlobal);
			// Copy the data								
			memcpy( pGlobalData, pData, dataLength );
			TotaldataLength = dataLength;			
		}	  
	}	// pos

  	GlobalUnlock( hGlobal );
	*phGlobal = hGlobal;
	
#endif
	return TRUE;
}


#if 0
/*	CPBaseDataSource method that renders the list of objects to
			Print Shop chunky data storage.

	@rdesc	TRUE on success.  FALSE on failure.
*/
BOOL CPBaseDataSource::RenderPSDChunk(
	HGLOBAL* phGlobal //,				// @parm A pointer to be given the address
									//		 of the memory handle with the data.
//	CFrogOleDataList* pObjList )	// @parm The list of objects to render.
	)
{
	// Create a memory storage
	RBufferStream stream;
	RStorage	parentStorage( &stream );
	RChunkStorage storage( &parentStorage );

	// Write the components to the storage
	for( POSITION pos = pObjList->GetHeadPosition(); pos != NULL; )
	{
		CFrogComponentCell* pObj = dynamic_cast<CFrogComponentCell *>(pObjList->GetNext(pos));
		//we should never get here Everything in the list should be a component
		ASSERT(pObj);
		if (pObj == NULL)
			break;		// This object didn't pass muster, but the next might.

		// We are in 96 dpi space, Print Shop and PressWriter live in TWIPS.
		// Therefore we must convert from our logical space to TWIPS prior to
		// putting out the component chunk data.
		pObj->Scale( 1440.0 / kSystemDPI );

		// Create a new chunk for the component
//		storage.AddChunk( kComponent );

		// Write the component itself
		pObj->WriteChunk( storage );

		// Get back to the top level
//		storage.SelectParentChunk( );

		// Now put the object back into the original space for any other renders
		pObj->Scale( kSystemDPI / 1440.0 );
	}

	// Get the size and data of the storage
	DWORD size;
	BYTE* pBuffer = storage.GetCurrentChunkBuffer( size );
	*phGlobal = ::PointerToHandle( pBuffer, size );
	
	return TRUE;
}
#endif


/* Allocates a list and fills it with copied object records, but only those
 *	records in the main object list which are currently selected.
 */
CPtrList *CPBaseDataSource::CreateSelectedList()
{
	// Build a list of objects to be dragged.
	CPtrList *plistSelected = new CPtrList();

	// Build our list of selected objects.
	for (	PMGPageObject* pObject = m_pDoc->selected_objects();
			pObject != NULL;
			pObject = pObject->next_selected_object())
	{
		plistSelected->AddTail( pObject );
	}

	return plistSelected;
}

/* Deallocates a list generated via a call to CreateSelectedList above.  This
 *	Removes all of the items from this list and deletes the list itself.
 */
void CPBaseDataSource::ReleaseSelectedList( CPtrList *pList )
{
	// Clear the list, but don't delete the actual items since they are part of
	// the actual database!
	pList->RemoveAll();

	// Now delete the memory allocated to the list pointer itself.
	delete pList;
}