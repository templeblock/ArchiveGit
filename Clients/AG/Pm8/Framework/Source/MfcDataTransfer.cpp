// ****************************************************************************
//
//  File Name:			MfcDataTransfer.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the MFC data transfer classes
//
//  Portability:		MFC dependent
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
//  $Logfile:: /PM8/Framework/Source/MfcDataTransfer.cpp                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"MfcDataTransfer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Utilities.h"
#include "ApplicationGlobals.h"
#include "CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "WindowView.h"
#include "Mainframe.h"
#include "StandaloneApplication.h"

// ****************************************************************************
//
//  Function Name:	::FillFormatEtc( )
//
//  Description:		Fills in a FORMATETC structure
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void FillFormatEtc( FORMATETC& formatetc, YDataFormat dataFormat )
	{
	formatetc.cfFormat = static_cast<CLIPFORMAT>( dataFormat );
	formatetc.ptd = NULL;
	formatetc.dwAspect = DVASPECT_CONTENT;
	formatetc.lindex = -1;

	switch( dataFormat )
		{
		case kEnhancedMetafileFormat :
			formatetc.tymed = TYMED_ENHMF;
			break;

		case kPictureFormat :
			formatetc.tymed = TYMED_MFPICT;
			break;

		case kBitmapFormat :
			formatetc.tymed = TYMED_GDI;
			break;

		default :
			formatetc.tymed = TYMED_HGLOBAL;
		}
	}

// ****************************************************************************
//
//  Function Name:	::FillStgMedium( )
//
//  Description:		Fills in a STGMEDIUM structure
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void FillStgMedium( STGMEDIUM& stgmedium, YDataFormat dataFormat, HANDLE handle )
	{
	switch( dataFormat )
		{
		case kEnhancedMetafileFormat :
			stgmedium.tymed = TYMED_ENHMF;
			stgmedium.hEnhMetaFile = static_cast<HENHMETAFILE>( handle );
			break;

		case kPictureFormat :
			stgmedium.tymed = TYMED_MFPICT;
			stgmedium.hMetaFilePict = static_cast<HMETAFILEPICT>( handle );
			break;

		case kBitmapFormat :
			stgmedium.tymed = TYMED_GDI;
			stgmedium.hBitmap = static_cast<HBITMAP>( handle );
			break;

		default :
			stgmedium.tymed = TYMED_HGLOBAL;
			stgmedium.hGlobal = handle;
		}
	}

// ****************************************************************************
//
//  Function Name:	RMFCDataTransferSource::RMFCDataTransferSource( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMFCDataTransferSource::RMFCDataTransferSource( COleDataObject* pOleDataObject )
   : m_pOleDataObject( pOleDataObject ),
	  m_fEnumerating( FALSE )
	{
	TpsAssert( m_pOleDataObject, "NULL data object." );
	}

// ****************************************************************************
//
//  Function Name:	RMFCDataTransferSource::GetData( )
//
//  Description:		Called to retrieve data of the specified format from this
//							data transfer protocol. The caller takes ownership of the
//							returned memory.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RMFCDataTransferSource::GetData( YDataFormat dataFormat, uBYTE*& pData, YDataLength& dataLength ) const
	{
	// Retrieve the global handle
	HGLOBAL hGlobal = m_pOleDataObject->GetGlobalData( static_cast<CLIPFORMAT>( dataFormat ) );

	//	If the Handle was not retrieved, there must not be this format so return pData = NULL and dataLength = 0
	if ( hGlobal )
		{
		// Lock down the global memory block and get its size
		uBYTE* pGlobalMem = static_cast<uBYTE*>( ::GlobalLock( hGlobal ) );
		if( !pGlobalMem )
			throw kMemory;

		dataLength = ::GlobalSize( hGlobal );

		// Allocate enough memory for the clipboard data
		pData = new uBYTE[ dataLength ];

		// Copy the memory
		memcpy( pData, pGlobalMem, dataLength );

		// Unlock and release the global handle
		::GlobalUnlock( hGlobal );
		::GlobalFree( hGlobal );
		}
	else
		{
		pData			= NULL;
		dataLength	= 0;
		}
	}

// ****************************************************************************
//
//  Function Name:	RMFCDataTransferSource::GetData( )
//
//  Description:		Called to retrieve data of the specified format from this
//							data transfer protocol. The caller takes ownership of the
//							returned handle.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
HNATIVE RMFCDataTransferSource::GetData( YDataFormat dataFormat ) const
	{
	// Make sure data of the requested type is available
	TpsAssert( IsFormatAvailable( dataFormat ), "Data of the requested type is not available." );

	FORMATETC formatetc;
	::FillFormatEtc( formatetc, dataFormat );

	STGMEDIUM stgmedium;
	::FillStgMedium( stgmedium, dataFormat, NULL );

	// Return the global handle
	if( m_pOleDataObject->GetData( static_cast<CLIPFORMAT>( dataFormat ), &stgmedium, &formatetc ) )
		return stgmedium.hGlobal;

	throw kClipboardError;
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RMFCDataTransferSource::IsFormatAvailable( )
//
//  Description:		Determines if the specified data format is available from
//							this data source.
//
//  Returns:			TRUE if data is available in the specified format
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMFCDataTransferSource::IsFormatAvailable( YDataFormat dataFormat ) const
	{
	FORMATETC formatetc;
	::FillFormatEtc( formatetc, dataFormat );
	return static_cast<BOOLEAN>( m_pOleDataObject->IsDataAvailable( static_cast<CLIPFORMAT>( dataFormat ), &formatetc ) );
	}

// ****************************************************************************
//
//  Function Name:	RMFCDataTransferSource::BeginEnumFormats( )
//
//  Description:		Sets this data transfer object up to enumerate available
//							data formats
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMFCDataTransferSource::BeginEnumFormats( ) const
	{
	TpsAssert( m_fEnumerating == FALSE, "Already enumerating." );

	m_pOleDataObject->BeginEnumFormats( );
	const_cast<RMFCDataTransferSource*>( this )->m_fEnumerating = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RMFCDataTransferSource::GetNextFormat( )
//
//  Description:		Retrieves the next data format available. Formats are
//							returned in decreasing order of priority.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMFCDataTransferSource::GetNextFormat( YDataFormat& dataFormat ) const
	{
	TpsAssert( m_fEnumerating == TRUE, "Not enumerating. Please call BeginEnumFormats( ) first." );

	FORMATETC formatetc;
	
	// Loop until we find data we can handle, or until we are done enumerating
	while( 1 )
		{
		// Get the next available format
		if( m_pOleDataObject->GetNextFormat( &formatetc ) )
			{
			// Check the media type for something we can handle.
			DWORD tymed = formatetc.tymed;
			if( ( tymed & TYMED_HGLOBAL ) == TYMED_HGLOBAL || ( tymed & TYMED_GDI ) == TYMED_GDI ||
				 ( tymed & TYMED_MFPICT ) == TYMED_MFPICT || ( tymed & TYMED_ENHMF ) == TYMED_ENHMF )
				{
				dataFormat = formatetc.cfFormat;
				return TRUE;
				}
			}
		else
			{
			const_cast<RMFCDataTransferSource*>( this )->m_fEnumerating = FALSE;
			return FALSE;
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RMFCDataTransferTarget::RMFCDataTransferTarget( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMFCDataTransferTarget::RMFCDataTransferTarget( RDataRenderer* pDataRenderer )
	: m_pOleDataSource( pDataRenderer )
	{
	if( !pDataRenderer )
		{
		m_fDelayedRenderingAvailable = FALSE;
		m_pOleDataSource = new COleDataSource;
		}

	else
		{
		m_fDelayedRenderingAvailable = TRUE;
		}
	}

// ****************************************************************************
//
//  Function Name:	RMFCDataTransferTarget::SetData( )
//
//  Description:		Sets data of the specified type. The caller retains
//							ownership of pData
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMFCDataTransferTarget::SetData( YDataFormat dataFormat, const uBYTE* pData, YDataLength dataLength )
	{
	TpsAssert( pData != NULL, "NULL data pointer. Use PromiseData( ) to implement delayed rendering." );
	TpsAssert( dataLength > 0, "No data." );

	try
		{
		HGLOBAL hGlobal = ::PointerToHandle( pData, dataLength );
		m_pOleDataSource->CacheGlobalData( static_cast<CLIPFORMAT>( dataFormat ), hGlobal );
		}

	catch( YException exception )
		{
		::ReportException( exception );
		}
	}

// ****************************************************************************
//
//  Function Name:	RMFCDataTransferTarget::SetData( )
//
//  Description:		Sets data of the specified type. The caller gives up 
//							ownership of the handle
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMFCDataTransferTarget::SetData( YDataFormat dataFormat, HNATIVE handle )
	{
	STGMEDIUM stgmedium;
	::FillStgMedium( stgmedium, dataFormat, handle );

	FORMATETC formatetc;
	::FillFormatEtc( formatetc, dataFormat );

	m_pOleDataSource->CacheData( static_cast<CLIPFORMAT>( dataFormat ), &stgmedium, &formatetc );
	}

// ****************************************************************************
//
//  Function Name:	RMFCDataTransferTarget::PromiseData( )
//
//  Description:		Called to promise that data of the specified type is
//							available through delayed rendering.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMFCDataTransferTarget::PromiseData( YDataFormat dataFormat )
	{
	TpsAssert( m_fDelayedRenderingAvailable == TRUE, "Delayed rendering not available without a data renderer." );

	FORMATETC formatetc;
	::FillFormatEtc( formatetc, dataFormat );

	m_pOleDataSource->DelayRenderData( static_cast<CLIPFORMAT>( dataFormat ), &formatetc );
	}

// ****************************************************************************
//
//  Function Name:	RRenaissanceOleDataSource::OnRenderGlobalData( )
//
//  Description:		Called by MFC to retrieve data through delayed rendering
//
//  Returns:			TRUE if successful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RDataRenderer::OnRenderData( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium )
	{
	if( lpStgMedium->tymed != TYMED_NULL )
		DataRendererBaseClass::OnRenderData( lpFormatEtc, lpStgMedium );

	// Create a COleDataObject and attach ourself as the IDataObject interface
	COleDataObject oleDataObject;
	oleDataObject.Attach( &m_xDataObject, FALSE );

	// Create a Renaissance data source from the COleDataObject
	RMFCDataTransferSource dataSource( &oleDataObject );

	// Create a data transfer target
	RMemoryDataTransfer dataTransfer;

	// Call Render data to get the data. This is a cross platform method
	if( RenderData( &dataSource, &dataTransfer, lpFormatEtc->cfFormat ) )
		{
		::FillStgMedium( *lpStgMedium, lpFormatEtc->cfFormat, dataTransfer.GetData( lpFormatEtc->cfFormat ) );
		if( lpStgMedium->hGlobal )
			return TRUE;
		}

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RRenaissanceOleDropSource::GiveFeedback( )
//
//  Description:		Displays the appropriate cursor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
SCODE RRenaissanceOleDropSource::GiveFeedback( DROPEFFECT dropEffect )
	{
	SCODE result = NOERROR;

	if( ( dropEffect & DROPEFFECT_MOVE ) == DROPEFFECT_MOVE )
		::GetCursorManager( ).SetCursor( CURSOR_RESOURCE_MOVE );

	else if( ( dropEffect & DROPEFFECT_COPY ) == DROPEFFECT_COPY )
		::GetCursorManager( ).SetCursor( CURSOR_RESOURCE_COPY );

	else if( ( dropEffect & DROPEFFECT_NONE ) == DROPEFFECT_NONE )
		::GetCursorManager( ).SetCursor( CURSOR_RESOURCE_SLASH );

	else
		{
		::GetCursorManager( ).ClearCursor( );
		result =  COleDropSource::GiveFeedback( dropEffect );
		}

	return result;
	}

// ****************************************************************************
//
//  Function Name:	RRenaissanceOleDropTarget::OnDragEnter( )
//
//  Description:		Called by the framework when the cursor is first dragged
//							into the window
//
//  Returns:			The effect that would result if a drop were attempted at
//							the location specified by point
//
//  Exceptions:		None
//
// ****************************************************************************
//
DROPEFFECT RRenaissanceOleDropTarget::OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
	{
	::GetInDragDrop( ) = TRUE;

	TpsAssertIsObject( RWindowView, pWnd );
	RWindowView* pView = dynamic_cast<RWindowView*>( pWnd );

	// Create a data source
	RMFCDataTransferSource dataSource( pDataObject );

	// Convert the point to logical units
	RRealPoint mousePoint( point.x, point.y );
	::DeviceUnitsToLogicalUnits( mousePoint, *pView );
	mousePoint = pView->ConvertPointToLocalCoordinateSystem( mousePoint );

	// Get the lowest hit view which can incorporate this data
	RRealPoint localPoint;
	m_pDragDropView = pView->GetLowestViewSupportingData( dataSource, mousePoint, localPoint );

	// Call the cross platform method
	if( m_pDragDropView )
		return m_pDragDropView->OnXDragEnter( dataSource, dwKeyState, localPoint );
	else
		return kDropEffectNone;
	}

// ****************************************************************************
//
//  Function Name:	RRenaissanceOleDropTarget::OnDragOver( )
//
//  Description:		Called by the framework during a drag operation when the
//							mouse is moved over the drop target window.
//
//  Returns:			Type of drop that would occur if the user dropped the
//							object at this position.
//
//  Exceptions:		None
//
// ****************************************************************************
//
DROPEFFECT RRenaissanceOleDropTarget::OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
	{
	TpsAssertIsObject( RWindowView, pWnd );
	RWindowView* pView = dynamic_cast<RWindowView*>( pWnd );

	// Create a data source
	RMFCDataTransferSource dataSource( pDataObject );

	// Convert the point to logical units
	RRealPoint mousePoint( point.x, point.y );
	::DeviceUnitsToLogicalUnits( mousePoint, *pView );
	mousePoint = pView->ConvertPointToLocalCoordinateSystem( mousePoint );

	// Get the lowest hit view which can incorporate this data
	RRealPoint localPoint;
	RView* pLowestHitView = pView->GetLowestViewSupportingData( dataSource, mousePoint, localPoint );

	// If this is the same view as before, just call it
	if( pLowestHitView == m_pDragDropView && pLowestHitView != NULL )
		return m_pDragDropView->OnXDragOver( dataSource, dwKeyState, localPoint );

	// Otherwise, notify the old view that it was left, and the new view that it
	// has been entered
	else
		{
		if( m_pDragDropView )
			m_pDragDropView->OnXDragLeave( );

		m_pDragDropView = pLowestHitView;

		if( m_pDragDropView )
			return m_pDragDropView->OnXDragEnter( dataSource, dwKeyState, localPoint );

		else
			return kDropEffectNone;
		}
	}

// ****************************************************************************
//
//  Function Name:	RRenaissanceOleDropTarget::OnDragLeave( )
//
//  Description:		Called by the framework during a drag operation when the
//							mouse is moved out of the valid drop area for that window.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRenaissanceOleDropTarget::OnDragLeave( CWnd* )
	{
	// Notify the current drag-drop view that we are leaving
	if( m_pDragDropView )
		m_pDragDropView->OnXDragLeave( );

	::GetInDragDrop( ) = FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RRenaissanceOleDropTarget::OnDrop( )
//
//  Description:		Called by the framework when the user releases a data
//							object over a valid drop target.
//
//  Returns:			Nonzero if the drop was successful; otherwise 0
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RRenaissanceOleDropTarget::OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
	{
	TpsAssertIsObject( RWindowView, pWnd );
	RWindowView* pView = dynamic_cast<RWindowView*>( pWnd );

	// Create a data source
	RMFCDataTransferSource dataSource( pDataObject );

	// Convert the point to logical units
	RRealPoint mousePoint( point.x, point.y );
	::DeviceUnitsToLogicalUnits( mousePoint, *pView );
	mousePoint = pView->ConvertPointToLocalCoordinateSystem( mousePoint );

	// Get the lowest hit view which can incorporate this data
	RRealPoint localPoint;
	RView* pLowestHitView = pView->GetLowestViewSupportingData( dataSource, mousePoint, localPoint );
	TpsAssert( pLowestHitView != NULL, "Why are we here?" );

	// If this is the same view as before, just call it
	if( pLowestHitView == m_pDragDropView )
		m_pDragDropView->OnXDrop( dataSource, localPoint, dropEffect );

	// Otherwise, notify the old view that it was left, and the new view that it
	// has been entered, and dropped
	else
		{
		if( m_pDragDropView )
			m_pDragDropView->OnXDragLeave( );

		pLowestHitView->OnXDragEnter( dataSource, 0, localPoint );
		pLowestHitView->OnXDrop( dataSource, localPoint, dropEffect );
		}

	// Per Windows UI guidelines, make this the foreground, active window
	pView->ActivateView( );

	// Force an idle, so various UI can be updated
	static_cast<RMainframe*>( AfxGetMainWnd( ) )->ForceIdle( );

	::GetInDragDrop( ) = FALSE;

	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RMFCDataTransferSource::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMFCDataTransferSource::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RMFCDataTransferSource, this );
	}

// ****************************************************************************
//
//  Function Name:	RMFCDataTransferTarget::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMFCDataTransferTarget::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RMFCDataTransferTarget, this );
	ASSERT_VALID( m_pOleDataSource );
	}

#endif	//	TPSDEBUG
