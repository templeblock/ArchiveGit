/*	
	$Header: /PM8/App/DragDataSource.cpp 1     3/03/99 6:05p Gbeddow $

	Derived class to handle delayed render for Drag Drop operations.

	Copyright 1998 The Learning Company, Inc., all rights reserved

	Revision History:

	$Log: /PM8/App/DragDataSource.cpp $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     1/25/99 3:38p Rgrenfell
// Added support for object descriptors in Drag/Drop
// 
// 1     1/07/99 5:07p Rgrenfel
// New files to allow Drag out of PM
*/

#include "stdafx.h"
#include "DragDataSource.h"
#include "PmwDoc.h"
#include "clip.h"						// Clipboard format definitions.


/*	@mfunc <c CPDragDataSource> class constructor.
*/
CPDragDataSource::CPDragDataSource():
	CPBaseDataSource() //,
//	m_pObjList( NULL )
{
}


/*	@mfunc <c CPDragDataSource> class destructor.  This cleans up the internal
			storage of the provided object list.
*/
CPDragDataSource::~CPDragDataSource()
{
//	delete m_pObjList;
}


#if 0
/*	CPDragDataSource method that provides a dynamically allocated memory file
	filled with the data from the internal object list.  The memory allocated is
	not freed by this class.  It is the callers responsibility to deallocate
	this memory.

	Returns : TRUE on success, FALSE on failure.
*/
BOOL CPDragDataSource::GetHandle(
	HANDLE* pHandle )	// A pointer to be given a handle of the copy of
							// the data which is to be passed for Drag/Drop.
{
	HANDLE hData = Serialize( m_pObjList );
	ASSERT( hData );
	if( hData == NULL )
		return FALSE;
	
	*pHandle = hData;
	return TRUE;
}
#endif


/* Checks the format and medium of the data requested and calls the appropriate
	render method to allow for the correct type of data to be rendered.
*/
BOOL CPDragDataSource::OnRenderData(
	LPFORMATETC lpFormatEtc,
	LPSTGMEDIUM lpStgMedium)
{
	if ((lpFormatEtc->tymed & TYMED_ENHMF) ||
		 (lpFormatEtc->tymed & TYMED_MFPICT) ||
		 (lpFormatEtc->tymed & TYMED_GDI) )
	{
		// attempt HGLOBAL delay render hook
		HGLOBAL hGlobal = lpStgMedium->hGlobal;
		if (OnRenderGlobalData(lpFormatEtc, &hGlobal))
		{
			ASSERT(hGlobal != NULL);
			lpStgMedium->tymed = lpFormatEtc->tymed;
			// Although we could work to cast this in a switch, it is defined as
			// a union, so it would simply add code bloat for no gain.
			lpStgMedium->hGlobal = hGlobal;
			return TRUE;
		}

//		// attempt CFile* based delay render hook
//		CSharedFile file;
//		if (lpStgMedium->tymed == TYMED_HGLOBAL)
//		{
//			ASSERT(lpStgMedium->hGlobal != NULL);
//			file.SetHandle(lpStgMedium->hGlobal, FALSE);
//		}
//		if (OnRenderFileData(lpFormatEtc, &file))
//		{
//			lpStgMedium->tymed = TYMED_HGLOBAL;
//			lpStgMedium->hGlobal = file.Detach();
//			ASSERT(lpStgMedium->hGlobal != NULL);
//			return TRUE;
//		}
//		if (lpStgMedium->tymed == TYMED_HGLOBAL)
//			file.Detach();
	}
	else
	{
		return COleDataSource::OnRenderData(lpFormatEtc, lpStgMedium);
	}

	return FALSE;
}	


/*	CPDragDataSource method that renders the data into the specified format and
	gives it back to the caller in phGlobal.

	The handle created is not cleaned up by this class and if it isn't
	given over to a self cleaning mechanism (such as the clipboard),
	this memory should be cleaned up by the caller.

	Returns : TRUE on success, FALSE on failure.
*/
BOOL CPDragDataSource::OnRenderGlobalData(
	LPFORMATETC lpFormatetc,	// The format to render the data in.
	HGLOBAL* phGlobal)			// A handle pointer to contain the address
										// of the handle for the rendered data.
{
	BOOL bResult = FALSE;

	// Let the document know that a render is active.
	m_pDoc->SetDropRenderActive();

//	if( lpFormatetc->cfFormat == kComponentFormat )
//		bResult = RenderPSDChunk( phGlobal, m_pObjList );
	
	if( lpFormatetc->cfFormat == CF_METAFILEPICT )
		bResult = RenderMetafile( phGlobal /*, m_pObjList */ );
	
	else if( lpFormatetc->cfFormat == CF_ENHMETAFILE )
		bResult = RenderEnhMetafile( phGlobal /*, m_pObjList */ );

	else if (lpFormatetc->cfFormat == CF_BITMAP)
	{
		bResult = RenderBitmap( phGlobal );
	}
	else if (lpFormatetc->cfFormat == CF_DIB)
	{
		bResult = RenderDIB( phGlobal );
	}
	else if (lpFormatetc->cfFormat == Clipboard::m_cfObjectDescriptor)
	{
		bResult = RenderObjectDescriptor( phGlobal );
	}
	
//	else if( lpFormatetc->cfFormat == kTextFormat )
//		bResult = RenderText( phGlobal, m_pObjList );
	
//	else if( lpFormatetc->cfFormat == kRTFFormat )
//		bResult = RenderRTF( phGlobal, m_pObjList );

	else
	{
		TRACE( "No legal format was requested.\n" );
		ASSERT( 0 );
	}

	m_pDoc->ClearDropRenderActive();

	return bResult;
}


#if 0
/* CPDragDataSource> method that initializes the drag source with
	the list of objects being dragged.
*/
void CPDragDataSource::Init(
	CPOleDataList *pList )	// The list of objects to drag.
{
	// If we already have a list, throw it away and start fresh.
	if (m_pObjList != NULL)
		delete m_pObjList;

	// Create a new list.
	m_pObjList = new CPOleDataList;
	// If we failed to get memory, simply return.
	if (m_pObjList == NULL)
	{
		ASSERT( 0 );
		return;
	}

	// Walk the supplied list and copy the contents into our new list.
	for (POSITION pos = pList->GetHeadPosition(); pos != NULL; )
	{
		CBObject *pObj = pList->GetNext(pos);
		if (pObj != NULL)
			m_pObjList->AddTail( pObj );
	}
}
#endif