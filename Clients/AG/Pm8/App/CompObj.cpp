/*
// $Header: /PM8/App/CompObj.cpp 2     3/08/99 1:15p Rgrenfel $
//
// Component object routines.
//
// $Log: /PM8/App/CompObj.cpp $
// 
// 2     3/08/99 1:15p Rgrenfel
// Reworked the original_dims method to provide accurate dimmensions which
// are not dependent on the current bounding rectangle.
// 
// 35    3/05/99 6:49p Rgrenfel
// Reworked the original_dims method to provide a consistent value, not
// one derived from the bounding rect.  Also fixed up GetDefaultSize to
// attempt to get legitamate data from the component before using its
// table driven method.
// 
// 34    3/01/99 2:06p Lwilson
// Added code to handle a transparent shadow color.
// 
// 33    2/26/99 8:40p Rgrenfel
// Supported the mechanism to get the original object's data path and
// added support for CGM graphic components.
// 
// 32    2/25/99 3:27p Lwilson
// Changed component creation to use a pointer to a CCreationData class or
// variant for in place of an RComponentDocument pointer.
// 
// 31    2/17/99 1:52p Lwilson
// Removed the call to the component record's EmbedComponent() method from
// write_data(), as the component record now stores the component data as
// part of the record itself.
// 
// 30    2/11/99 10:09a Lwilson
// Added GetContextMenuIndex() to determine which context menu to display
// for the embedded component.
// 
// 29    2/10/99 5:06p Rgrenfel
// Added protected method to allow the detachment of the embedded
// component document.
// 
// 28    2/10/99 1:29p Lwilson
// Added changed_image() override to re-calc the component bounds and to
// invalidate the render cache.  Also, updated the calc() method to call
// the component's ClearFlippedFlags() to keep the flipped flags in sync.
// 
// 27    2/10/99 12:11p Lwilson
// Updated SetComponentData to accept flags to determine whether or not
// the current component's attributes and/or size should be kept.
// 
// 26    2/09/99 3:29p Lwilson
// To fix some serialization problems, the call to EmbedComponent was
// moved from the component record to the component object.
// 
// 25    2/04/99 5:57p Rgrenfel
// Allowed the detection that the raw object data was in JPEG format.
// 
// 24    2/04/99 11:58a Rgrenfel
// Added methods for determining the raw image size and to allow the
// export of the raw image.
// 
// 23    2/04/99 10:09a Lwilson
// Added graphic object conversion
// 
// 22    2/02/99 1:20p Gbeddow
// fix bug in LoadFromPretzelCollection member function name change that
// Rich made
// 
// 21    2/01/99 4:35p Lwilson
// Added CreateFromObject method to support converting PM objects into
// component objects.  Also implemented the conversion for drawing,
// ellipse, and rectangle objects.
// 
// 20    2/01/99 2:22p Cschendel
// In addoutline() removed translation of empty paths and duplicate points
// 
// 19    1/29/99 3:08p Lwilson
// Improved tracking feedback by using component's
// RenderInternalTrackingFeedback 
// 
// 18    1/28/99 4:53p Jfleischhauer
// removed image-specific stuff from SetComponentData(); comparisons now
// done by bounding rect
// 
// 17    1/27/99 1:59p Lwilson
// Added shadow support and tracker feedback rendering.
// 
// 16    1/25/99 3:44p Rgrenfell
// Added support for requesting if the component is an animated GIF image.
// 
// 15    1/25/99 9:58a Lwilson
// Initial path component support
// 
// 14    1/20/99 4:33p Jfleischhauer
// added resizing to SetComponentData()
// 
// 13    1/20/99 1:49p Lwilson
// Added calc() call to RestoreState() so the component bounds will be
// restored correctly.
// 
// 12    1/19/99 4:05p Cschendel
// fixed comment header
// 
//  11    1/19/99 4:01p Cschendel
//  changes to AddOutline() for irregular text wrap support
//  
// 10    1/19/99 2:57p Lwilson
// Overwrote calc() method to set the components bounding box when ever
// it's position or size changes.
// 
// 9     1/13/99 4:59p Cschende
// overrode AddOutline to support giving a non-rectangular shape used for
// irregular text wrap
// 
// 8     1/11/99 1:32p Gbeddow
// support for reading/writing Print Shop square, row & column vector
// graphics in Pretzel collection files
// 
// 7     12/17/98 8:57a Rgrenfel
// Changed the name for getting an image document to a generic GetDocument
// call.
// 
// 6     12/16/98 6:22p Rgrenfel
// Added a method to allow a component object to create an image document
// from a Graphic Creation structure.
// 
// 5     12/15/98 3:37p Lwilson
// Initial support for PS component editing.
// 
// 4     12/14/98 1:06p Lwilson
// Added default sizing support for PS components.
// 
// 3     12/10/98 12:56p Lwilson
// Implemented is_opaque() and IsEmpty() member functions.  Fixed
// selection handle update problem, as well as updated drawing surface to
// account for printing.
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
#include "CompObj.h"
#include "CompRec.h"
#include "Utils.h"
#include "PMWDoc.h"
#include "ObjectSize.h"
#include "PMWColl.h"	// Collection manager definition.
#include "outpath.h"
#include "drawobj.h" // Object conversion
#include "grafobj.h" // Object conversion

#include "CreateData.h"		// Creation data structures

// Framework support
#include "ApplicationGlobals.h"
#include "ComponentManager.h"
#include "ComponentTypes.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "ComponentCollection.h"
#include "DcDrawingSurface.h"
#include "MfcDataTransfer.h"
#include "ChunkyStorage.h"
#include "Path.h"
#include "BitmapImage.h"
#include "SoftShadowSettings.h"
#include "GifAnimator.h"

// Interface support
#include "PathInterface.h"
#include "ImageInterface.h"
#include "PropertyInterface.h"

const	YRealDimension kMinObjectAspectThreshHold = ((1440 * 3) / 4);

static CPComponentToContextMenuMap _cContextMenuMap;

CPComponentToContextMenuMap::CPComponentToContextMenuMap()
{
	SetAt( kImageComponent,				(void *) 2L );
	SetAt( kSquareGraphicComponent,	(void *) 2L );
	SetAt( kRowGraphicComponent,		(void *) 2L );
	SetAt( kColumnGraphicComponent,	(void *) 2L );
	SetAt( kCGMGraphicComponent,		(void *) 2L );
	SetAt( kPathComponent,				(void *) 6L );
}

ComponentObjectState::~ComponentObjectState()
{
	if (comp_record.recno)
	{
		m_pDatabase->free_component_record( comp_record.recno );
	}
}

/*
// The constructor for the component object.
*/
ComponentObject::ComponentObject( DB_OBJECT_TYPE type, ObjectDatabasePtr owner )
	: RectPageObject( type, owner )
{
	record.select_flags = 
		SELECT_FLAG_boundary     | 
		SELECT_FLAG_size_handles | 
		SELECT_FLAG_move_handle  | 
		SELECT_FLAG_rotate_handle;

	comp_record.recno = 0;
}

ComponentObject::~ComponentObject()
{
}

/*
// The creator for a component object.
*/
ERRORCODE ComponentObject::create( DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record )
{
	ComponentObject* object = NULL;
	ERRORCODE error = ERRORCODE_None;

	/* Create the frame object. */
	if ((object = new ComponentObject( type, owner )) == NULL)
	{
		error = ERRORCODE_Memory;
	}
	else if (creation_data)
	{
		error = object->SetComponentData( (CCreationData *) creation_data, FALSE );

		if (ERRORCODE_None != error)
		{
			delete object;
			object = NULL;
		}
	}

	*record = (PageObjectPtr)object;

	return error;
}

/*
// Destroy this object.
// The object is being removed from the database.
// Any and all sub-objects must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/
void ComponentObject::destroy(void)
{
	if (comp_record.recno)
		((PMGDatabase*) database)->free_component_record( comp_record.recno );
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/
ERRORCODE ComponentObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = RectPageObject::read_data(device)) == ERRORCODE_None)
	{
		if ((error = device->read_record(&comp_record, sizeof(comp_record))) == ERRORCODE_None)
		{
		#if 0
			// REVIEW: Is this necessary for components
			//
			/*
			// We want to update the rotate handle.
			// This can cause the file position to change within the device.
			// So we save and restore it around the UpdateRotateHandle() call.
			*/
			ST_DEV_POSITION pos;
			device->tell(&pos);
			UpdateRotateHandle();
			device->seek(pos, ST_DEV_SEEK_SET);
		#endif
		}
	}

	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/
ERRORCODE ComponentObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = RectPageObject::write_data(device)) == ERRORCODE_None)
	{
		error = device->write_record(&comp_record, sizeof(comp_record));
	}

	return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE ComponentObject::size_data(StorageDevicePtr device)
{
	return RectPageObject::size_data( device ) + device->size_record( sizeof(comp_record) );
}

/*
// Assign an object to this.
*/

ERRORCODE ComponentObject::assign(PageObjectRef sobject)
{
	ERRORCODE error;

	ComponentObject& object = (ComponentObject&) sobject;

	/* Copy the base object data. */
	if ((error = RectPageObject::assign( sobject )) == ERRORCODE_None)
	{
		/* Copy the specific component data. */
		comp_record = object.comp_record;

		if (database == object.database)
		{
			// Just increment the reference count
			((PMGDatabase*) database)->inc_component_record( comp_record.recno );
		}
		else
		{
			// Get the source component so we can duplicate it in the destination database.
			ComponentRecord* pCompRec = (ComponentRecord*) object.database->get_record(
				object.comp_record.recno, &error, RECORD_TYPE_Component );

			if (pCompRec)
			{
				// Duplicate the record. Extreme care must be taken to 
				// maintain the correct database at all times.
				ComponentRecord* pNewCompRec = (ComponentRecord *) pCompRec->duplicate(
					database, &error );

				if (pNewCompRec)
				{
					/* Set it in the graphic object. */
					comp_record.recno = pNewCompRec->Id();
					pNewCompRec->release();
				}

				pCompRec->release();
			}
		}

		/* Make sure the error array is fully allocated. */
//		allocate_error_array(-1);
	}

	return error;
}


/*
// Create a state for saving.
// We return a pointer to a new object. This object needs to be freed by
// the caller at some point.
*/
PageObjectState* ComponentObject::CreateState(void)
{
	return new ComponentObjectState;
}

/*
// Save our state.
*/

void ComponentObject::SaveState(PageObjectState* pState)
{
	ComponentObjectState* pMyState = (ComponentObjectState*)pState;

	INHERITED::SaveState(pState);

	pMyState->comp_record = comp_record;

	if (comp_record.recno)
	{
		PMGDatabase* pDatabase = pMyState->m_pDatabase;
		pDatabase->inc_component_record( comp_record.recno );
	}
}

/*
// Restore our state.
// The state passed in is NOT deleted by us. It is still the responsibility
// of the caller (or whomever).
*/

void ComponentObject::RestoreState(PageObjectState* pState)
{
	INHERITED::RestoreState( pState );

	ComponentObjectState* pMyState = (ComponentObjectState*) pState;
	DB_RECORD_NUMBER new_recno = pMyState->comp_record.recno;


	if (new_recno != comp_record.recno)
	{
//		changed_image();

		PMGDatabase* pDatabase = pMyState->m_pDatabase;

		/* In with the new. */
		if (new_recno)
		{
			pDatabase->inc_component_record( new_recno );
		}

		/* Out with the old. */
		if (comp_record.recno)
		{
			pDatabase->free_component_record( comp_record.recno );
		}

		comp_record = pMyState->comp_record;
	}
	else
	{
		calc();
	}
}

/*
// Get the properties for this object.
*/

void ComponentObject::GetObjectProperties(CObjectProperties& Properties)
{
	// Get the component view 
	ComponentRecord* pComponent = LockComponentRecord();
	if (!pComponent) return;

	RComponentView* pComponentView = pComponent->GetComponentView();
	pComponent->release();

	IObjectProperties* pIProperties = (IObjectProperties *) 
		pComponentView->GetInterface( kObjectPropertiesInterfaceId );

	if (pIProperties)
	{
		IFillColor* pIFillColor = NULL;
		if (pIProperties->FindProperty( kFillColorAttribInterfaceId, (RInterface **) &pIFillColor ))
		{
			Properties.SetFillForegroundColor( ColorFromRColor( pIFillColor->GetColor() ) );
			delete pIFillColor;
		}

		IOutlineAttrib* pIOutlineAttrib = NULL;
		if (pIProperties->FindProperty( kOutlineAttribInterfaceId, (RInterface **) &pIOutlineAttrib ))
		{
			OUTLINE_FORMAT ofs = pIOutlineAttrib->GetOutlineFormat();
			delete pIOutlineAttrib;

			SHORT nLineStyle = OutlineFormat::Inflate;
			SHORT nLineType  = OutlineFormat::Relative;
			CFixed nLineWidth = ofs.m_uwLineWeight;

			if (ofs.m_uwPenStyle == IOutlineAttrib::kNone)
			{
				nLineStyle = ofs.m_uwPenStyle;
			}
			else if (ofs.m_uwPenStyle == IOutlineAttrib::kHairLine)
			{
				nLineStyle = ofs.m_uwPenStyle;
				nLineType  = OutlineFormat::Absolute;
				nLineWidth = 0;
			}
			else if (ofs.m_uwPenStyle == IOutlineAttrib::kFixed)
			{
				nLineWidth = (ofs.m_uwLineWeight << 16);
				nLineType  = OutlineFormat::Absolute;
			}

			Properties.SetOutlineStyle( nLineStyle );
			Properties.SetOutlineForegroundColor( color_from_colorref( ofs.m_yPenColor ) );
			Properties.SetOutlineWidthType( nLineType );
			Properties.SetOutlineWidth( nLineWidth );
		}

		delete pIProperties;
	}

	// Shadow settings
	///////////////////

	RSoftShadowSettings rShadowSettings;
	pComponentView->GetShadowSettings( rShadowSettings );

	int nStyle = (rShadowSettings.m_fShadowOn ? ShadowFormat::Drop : ShadowFormat::ShadowNone);

	CFixed lXOffset = MakeFixed( cos(-rShadowSettings.m_fShadowAngle) * rShadowSettings.m_fShadowOffset );
	CFixed lYOffset = MakeFixed( sin(-rShadowSettings.m_fShadowAngle) * rShadowSettings.m_fShadowOffset );

	Properties.SetShadowStyle( nStyle );
	Properties.SetShadowOffsetType( ShadowFormat::Relative );
	Properties.SetShadowXOffset( lXOffset );
	Properties.SetShadowYOffset( lYOffset );
	Properties.SetShadowForegroundColor( color_from_colorref( rShadowSettings.m_fShadowColor ) );
}

/*
// Set the properties for this object.
*/

BOOL ComponentObject::SetObjectProperties(CObjectProperties& Properties)
{
	BOOLEAN fResult = FALSE;

	// Get the component view 
	ComponentRecord* pComponent = LockComponentRecord();
	if (!pComponent) return fResult;

	RComponentView* pComponentView = pComponent->GetComponentView();
	pComponent->release();

	IObjectProperties* pIProperties = (IObjectProperties *) 
		pComponentView->GetInterface( kObjectPropertiesInterfaceId );

	if (pIProperties)
	{

		IFillColor* pIFillColor = NULL;
		if (pIProperties->FindProperty( kFillColorAttribInterfaceId, (RInterface **) &pIFillColor ))
		{
			RColor color;
			
			if (Properties.ApplyFillForegroundColor( color ))
			{
				pIFillColor->SetColor( color);
				fResult = TRUE;
			}

			delete pIFillColor;
		}

		IOutlineAttrib* pIOutlineAttrib = NULL;
		if (pIProperties->FindProperty( kOutlineAttribInterfaceId, (RInterface **) &pIOutlineAttrib ))
		{
			COLOR color(0L);
			if (Properties.ApplyOutlineForegroundColor( color ))
			{
				pIOutlineAttrib->SetColor( RColorFromColor( color ) );
				fResult = TRUE;
			}


			SHORT nStyle;
			CFixed lWidth;

			if (Properties.ApplyOutlineStyle( nStyle ) && nStyle >= IOutlineAttrib::kNone && nStyle <= IOutlineAttrib::kHairLine)
			{
				// Currently only none, and hairline are supported
				pIOutlineAttrib->SetPenStyle( nStyle );
				fResult = TRUE;
			}
			else if (Properties.ApplyOutlineWidth( lWidth ))
			{
				SHORT nWidthType;
				Properties.ApplyOutlineWidthType( nWidthType );
				
				if (OutlineFormat::Absolute == nWidthType)
				{
					fResult = TRUE;
					lWidth  = (lWidth >> 16);
					pIOutlineAttrib->SetPenStyle( IOutlineAttrib::kFixed );
					pIOutlineAttrib->SetLineWeight( lWidth );
				}
			}

			delete pIOutlineAttrib;
		}

		delete pIProperties;
	}

	// Shadow settings
	///////////////////

	RSoftShadowSettings rShadowSettings;
	pComponentView->GetShadowSettings( rShadowSettings );

	SHORT nStyle, nOffsetType;

	if (Properties.ApplyShadowStyle( nStyle ))
	{
		fResult = TRUE;
		rShadowSettings.m_fShadowOn = nStyle == ShadowFormat::Drop;
		rShadowSettings.m_fShadowEdgeSoftness = 0.0;
		rShadowSettings.m_fShadowOpacity = 1.0;
	}

	if (Properties.ApplyShadowOffsetType( nOffsetType ))
	{
		if (ShadowFormat::Relative == nOffsetType)
		{
			fResult = TRUE;
			CFixed lXOffset, lYOffset;

			if (Properties.ApplyShadowXOffset( lXOffset ) && Properties.ApplyShadowYOffset( lYOffset ))
			{
				double flXOffset = MakeDouble( lXOffset );
				double flYOffset = MakeDouble( lYOffset );

				rShadowSettings.m_fShadowAngle  = -atan2( flYOffset, flXOffset );
				rShadowSettings.m_fShadowOffset = sqrt( flXOffset * flXOffset + flYOffset * flYOffset );
			}
		}
	}

	COLOR ShadowColor = color_from_colorref( rShadowSettings.m_fShadowColor );
	if (Properties.ApplyShadowForegroundColor( ShadowColor ))
	{
		fResult = TRUE;
		rShadowSettings.m_fShadowColor = colorref_from_color( ShadowColor );
		
		if (TRANSPARENT_COLOR == ShadowColor)
		{
			rShadowSettings.m_fShadowOn = FALSE;
		}
	}

	pComponentView->SetShadowSettings( rShadowSettings );

   return fResult;
}

/*
// Is this object empty?
*/
BOOL ComponentObject::IsEmpty()
{
   BOOL fResult = TRUE;

	ComponentRecord* pComponentRecord = LockComponentRecord();

   if(pComponentRecord)
	{
		if (pComponentRecord->GetComponentView())
			fResult = FALSE;

		pComponentRecord->release();
	}

   return fResult;
}

/*
// Is this object opaque?
*/

BOOL ComponentObject::is_opaque(RedisplayContextPtr rc)
{
	// TODO: this should probably be passed on to the component at 
	// some time, but for now, we will just check for a background
	// color.

	BOOL fResult = FALSE;

	try
	{
		// Get the component view to be rendered
		ComponentRecord* pComponent = LockComponentRecord();
		RComponentView* pComponentView = pComponent->GetComponentView();
		pComponent->release();

		if (pComponentView->HasBackgroundColor( ))
			fResult = TRUE;

	}
	catch (...)
	{
	}

	return fResult;
}

/*
// Recalc after a size or movement.
*/

VOID ComponentObject::calc( PBOX_PTR panel_world, FLAGS panel_flags )
{
	try
	{
		RectPageObject::calc( panel_world, panel_flags );

		// Get the component view 
		ComponentRecord* pComponent = LockComponentRecord();
		RComponentView* pComponentView = pComponent->GetComponentView();
		pComponent->release();

		// Compute the component's size
		//
		R2dTransform transform;
		transform.PostScale( (float) kSystemDPI / PAGE_RESOLUTION, (float) kSystemDPI / PAGE_RESOLUTION );

		PBOX box = GetUnrotatedDrawBound();
		RRealRect rBounds = RRealRect( box.x0, box.y0, box.x1, box.y1 ) * transform;
		
		// A component with a width or height equal
		// to 0 cannot be scaled.  Scaling is necessary
		// for tracking feedback rendering.
		if (!rBounds.Width())
			rBounds.m_Right = rBounds.m_Left + kSystemDPI;
		
		if (!rBounds.Height())
			rBounds.m_Bottom = rBounds.m_Top + kSystemDPI;

		pComponentView->SetBoundingRect( rBounds );
		pComponentView->ClearFlippedFlags( );

		// Flip the component as necessary
		//
		if (OBJECT_FLAG_xflipped & get_flags())
		{
			pComponentView->FlipHorizontal( pComponentView->GetBoundingRect( ).GetCenterPoint( ).m_x );
		}

		if (OBJECT_FLAG_yflipped & get_flags())
		{
			pComponentView->FlipVertical( pComponentView->GetBoundingRect( ).GetCenterPoint( ).m_y );
		}

		// Rotate the component as necessary
		//
		if (!AreFloatsEqual( get_rotation(), 0.0 ))
		{
			pComponentView->RotateAboutCenter( get_rotation() );
		}

		// Position the component at 0,0 as translation to it's appropriate place on
		// the device will happen through the render transformation.
		rBounds = pComponentView->GetBoundingRect( ).m_TransformedBoundingRect;
		pComponentView->Offset( RRealSize( -rBounds.m_Left, -rBounds.m_Top ), FALSE );
	}
	catch (...)
	{
	}
}

/*
// The image has changed.
*/
VOID ComponentObject::changed_image(VOID)
{
	RectPageObject::changed_image();

	// Many of the changes require the
	// recalculation of the component bounds.
	calc();

	// We should also invalidate the views render cache.
	RComponentView* pComponentView = GetComponentView();

	if (pComponentView)
		pComponentView->InvalidateRenderCache();
}

/*
// Determine the refresh bound.
*/
void ComponentObject::get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc)
{
	*refresh_bound = get_bound();

	try
	{
		// Get the component view 
		RComponentView* pComponentView = GetComponentView();

		if (pComponentView->HasShadow())
		{
			RRealSize softEffectOffsetSize;
			RRealRect rRect( refresh_bound->x0, refresh_bound->y0, refresh_bound->x1, refresh_bound->y1 );
			pComponentView->AdjustRectForSoftEffect( rRect, softEffectOffsetSize );

			// Assign new bounds (with a fudge factor of PAGE_RESOLUTION/10)
			refresh_bound->x0 = rRect.m_Left   - PAGE_RESOLUTION/10;
			refresh_bound->y0 = rRect.m_Top    - PAGE_RESOLUTION/10;
			refresh_bound->x1 = rRect.m_Right  + PAGE_RESOLUTION/10;
			refresh_bound->y1 = rRect.m_Bottom + PAGE_RESOLUTION/10;
		}
	}
	catch (...)
	{
	}
}

void ComponentObject::ToggleBounds(RedisplayContext* rc, POINT* p)
{
	RectPageObject::ToggleBounds(rc, p);
	YPenStyle penStyle = kSolidPen;

	// Show the bound if panning or cropping.
	if (get_refresh_flags() & (REFRESH_FLAG_panning|REFRESH_FLAG_cropping))
		penStyle = kDotPen;

	ComponentRecord* pComponent = LockComponentRecord();

	if (pComponent)
	{
		// Get the component view 
		RComponentView* pComponentView = pComponent->GetComponentView();
		pComponent->release();

		// Create a transform. 
		R2dTransform transform;
		transform.PostScale( 
			(float) rc->GetScaledXResolution() / kSystemDPI, 
			(float) rc->GetScaledYResolution() / kSystemDPI );

		// Adjust transform to account for possible resizing
		PBOX box = GetUnrotatedDrawBound();
		rc->pbox_to_screen( &box, FALSE );

		RRealSize rViewSize = pComponentView->GetReferenceSize() * transform;
		transform.PostScale( box.Width() / rViewSize.m_dx, box.Height() / rViewSize.m_dy );

		// Handle offset for cropping and any additional 
		// positioning based on the redisplay context.  Note,
		// the adjustment is in screen coordinates so post
		// translate the values as the matrix is set up for
		// device units based on the above transformations.
//		PBOX box = GetDrawBound();
//		rc->pbox_to_screen( &box, FALSE );
		transform.PostTranslate( box.x0, box.y0 );
		transform.PostRotateAboutPoint( box.CenterX(), box.CenterY(), get_rotation() );
	

		// Render the feedback
		///////////////////////

		// Create a drawing surface from  the RedisplayContext
		RDcDrawingSurface ds( FALSE, rc->is_printing );
		ds.Initialize( rc->destination_hdc, rc->AttributeDC() );
		
		ds.SetPenStyle( penStyle );
		ds.SetPenColor( RSolidColor( SELECT_COLOR ) );
		pComponentView->RenderInternalTrackingFeedback( ds, transform, *pComponentView );
		ds.RestoreDefaults();
		ds.DetachDCs();
	}
}

/*
// Return the original dimensions of an object.
*/

BOOL ComponentObject::original_dims(PPNT_PTR p)
{
	BOOL bSuccessfull = FALSE;

	ComponentRecord* pCompRec = LockComponentRecord();

	if (pCompRec)
	{
		// We are going to assume an 8x11 panel since we can't request the actual
		// panel from the document.
		CPmwDoc* pDoc = (CPmwDoc*)get_database()->GetDocument();
		PBOX boxPanelAssumed;
		if (pDoc)
		{
			// We have a document, so check the panel size.
			pDoc->get_panel_world(&boxPanelAssumed);
		}
		else
		{
			// We couldn't get at our document, so build an assumed size.
			boxPanelAssumed.x0 = 0;
			boxPanelAssumed.y0 = 0;
			boxPanelAssumed.x1 = 8*PAGE_RESOLUTION;
			boxPanelAssumed.y1 = 11*PAGE_RESOLUTION;
		}

		// OK, let's get the our default size.
		RRealSize sizeOriginal = GetDefaultSize( &boxPanelAssumed );

		// Inform the caller of the sizing data.
		p->x = scale_pcoord( sizeOriginal.m_dx, PAGE_RESOLUTION, kSystemDPI );
		p->y = scale_pcoord( sizeOriginal.m_dy, PAGE_RESOLUTION, kSystemDPI );

		bSuccessfull = TRUE;

		pCompRec->release();
	}

	return bSuccessfull;
}

/*
// Update a graphic object.
*/
UpdateStatePtr ComponentObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
{
	ASSERT(ustate == NULL);

	// Create a drawing surface from 
	// the RedisplayContext
	RDcDrawingSurface ds( FALSE, rc->is_printing );
	ds.Initialize( rc->destination_hdc, rc->AttributeDC() );

	try
	{
		////////////////////////////////////////
		// Set the component up for rendering //
		////////////////////////////////////////

		// Get the component view to be rendered
		ComponentRecord* pComponent = LockComponentRecord();
		RComponentView* pComponentView = pComponent->GetComponentView();
		pComponent->release();

		//////////////////////////////////////////////////////////////////
		// Determine the transformation matrix from the display context //
		//////////////////////////////////////////////////////////////////

		// Convert redisplay context's scaled 
		// Create a transform. 
		R2dTransform transform;
		transform.PostScale( 
			(float) rc->GetScaledXResolution() / kSystemDPI, 
			(float) rc->GetScaledYResolution() / kSystemDPI );

		// Handle offset for cropping and any additional 
		// positioning based on the redisplay context.  Note,
		// the adjustment is in screen coordinates so post
		// translate the values as the matrix is set up for
		// device units based on the above transformations.
		PBOX box = GetDrawBound();
		rc->pbox_to_screen( &box, FALSE );
		transform.PostTranslate( box.x0, box.y0 );

		// Apply the components transform
		pComponentView->ApplyTransform( transform, FALSE, rc->is_printing );

		//////////////////////////
		// Render the component //
		//////////////////////////
		
		BOOL f = ApplyClipRgn( rc );

		if (f != -1)
		{
			CRect crDraw;
			get_refresh_bound( &box, rc );
			rc->pbox_to_screen( &box, FALSE );
			rc->convert_pbox( &box, &crDraw, NULL );

			RIntRect rcRender( clip );
			rc->toggle( FALSE, clip );
			pComponentView->Render( ds, transform, rcRender );
			rc->toggle( TRUE, clip );

			// Restore the DC if we selected a clip region.
			RestoreClipRgn( rc, f );
		}
	}
	catch (...)
	{
	}

	// Detach the drawing surface DCs
	ds.DetachDCs( );

	return NULL;
}

/*
// Obtain the objects clip box from the given extent
*/
CRect ComponentObject::GetClipBox( RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip )
{
	PBOX DrawBound = GetDrawBound();
	PBOX Bound = get_bound();
	PBOX ClippedExtent;
	CRect crDraw;

	if (!IntersectBox(&ClippedExtent, extent, &Bound) || 
		!IntersectBox(&ClippedExtent, &ClippedExtent, &DrawBound))
	{
//		ASSERT(ustate == NULL);
//		delete ustate;			// Just in case.
//
//		return NULL;
		return crDraw;
	}

	PBOX box = ClippedExtent;
	rc->pbox_to_screen( &box, FALSE );

	if (!rc->convert_pbox( &box, &crDraw, NULL ))
	{
//		ASSERT(ustate == NULL);
//		delete ustate;			// Just in case.
//	
//		return NULL;
		return crDraw;
	}

	extent = &ClippedExtent;
	ASSERT(clip != NULL);

	if (clip != NULL)
	{
		if (!crDraw.IntersectRect(crDraw, clip))
		{
//			ASSERT(ustate == NULL);
//			delete ustate;			// Just in case.
//
//			return NULL;
			return crDraw;
		}
	}

	clip = &crDraw;

	return crDraw;
}

/*
// Lock the graphic record for this object.
// The caller must do a release().
*/
ComponentRecord* ComponentObject::LockComponentRecord()
{
	// We must have a record attached.
	if (!comp_record.recno)
	{
		return NULL;
	}

	// Lock and return the graphic record.
	return (ComponentRecord*) get_database()->get_record( comp_record.recno, NULL, RECORD_TYPE_Component );
}

// ****************************************************************************
//
//  Function Name:	GetDefaultSize( )
//
//  Description:		Gets the default size of this component slab
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize ComponentObject::GetDefaultSize( PBOX_PTR pPanelSize )
{
	RComponentView* pComponentView = GetComponentView();
	RComponentDocument* pComponentDoc = pComponentView->GetComponentDocument();

	// Determine transformation matrix 
	// for converting logical units.
	R2dTransform transform;
	transform.PostScale( (float) kSystemDPI / PAGE_RESOLUTION, (float) kSystemDPI / PAGE_RESOLUTION );
	RRealSize rSize = RRealSize( pPanelSize->Width(), pPanelSize->Height() ) * transform;

	// Attempt to read the default size from the object itself.  If it can
	// provide a legitimate size, than we are finished.  If not, then we need to
	// query an internal table and figure it out on our end.
	if (pComponentDoc->GetDefaultSize( rSize, &rSize ))
	{
		return rSize;
	}

	// We failed to get a size directly from the object, so create one...
	const YComponentType& componentType = pComponentDoc->GetComponentType( );
	RObjectSize::EObjectType eType = RObjectSize::GetObjectType( componentType );

	YRealDimension yAspectRatio = RObjectSize::GetAspectRatio( eType );
	RRealSize rMinSize( rSize );

	if( rSize.m_dx > rSize.m_dy )
	{
		rSize.m_dy *= RObjectSize::GetDefaultSizeLandscape( eType );
		rSize.m_dx = rSize.m_dy * yAspectRatio;

		//	if smaller panel dimension is less than 3/4", then make min object
		//	size double the normal requirement to keep objects from being too small
		//	on small labels.
		if( rMinSize.m_dx < kMinObjectAspectThreshHold )
			rMinSize.m_dx *= 2;

		rMinSize.m_dx *= RObjectSize::GetMinSizePortrait( eType );
		rMinSize.m_dy = rMinSize.m_dx / yAspectRatio;
	}
	else
	{
		rSize.m_dx *= RObjectSize::GetDefaultSizePortrait( eType );
		rSize.m_dy = rSize.m_dx / yAspectRatio;

		//	if smaller panel dimension is less than 3/4", then make min object
		//	size double the normal requirement to keep objects from being too small
		//	on small labels.
		if( rMinSize.m_dy < kMinObjectAspectThreshHold)
			rMinSize.m_dy *= 2;

		rMinSize.m_dy *= RObjectSize::GetMinSizeLandscape( eType );
		rMinSize.m_dx = rMinSize.m_dy * yAspectRatio;
	}

	//	Prevent min size from being bigger than default size. 
	// This can happen if min size is bigger than panel size.
	if (rMinSize.m_dx > rSize.m_dx || rMinSize.m_dy > rSize.m_dy)
	{
		rSize = rMinSize;
	}

	return rSize;
}

ERRORCODE ComponentObject::SetComponentData( RComponentDocument* pComponentDoc, WORD wFlags )
{
	CComponentData cData( pComponentDoc );

	return SetComponentData( &cData, wFlags );
}

ERRORCODE ComponentObject::SetComponentData( CCreationData* pData, WORD wFlags )
{
	DatabaseRecordPtr record = NULL;
	ERRORCODE error = database->new_record( RECORD_TYPE_Component, pData, &record );

	if (ERRORCODE_None == error)
	{
		if (comp_record.recno)
		{
			// set the bounding rect
			RComponentView* pOldComponentView = GetComponentView();
			RComponentView* pNewComponentView = ((ComponentRecord *) record)->GetComponentView();

			if( wFlags & kCopyAttributes )
				pNewComponentView->CopyViewAttributes( pOldComponentView );

			// if resizing to the bounds of the new component
			if( wFlags & kResetSize )
			{
				ComponentRecord* pRecord = LockComponentRecord();
				if( pRecord )
				{
					// get the unrotated bounds for this object
					PBOX urBounds = get_unrotated_bound();

					PCOORD urBoundsW = urBounds.Width();
					PCOORD urBoundsH = urBounds.Height();

					PCOORD urBoundsCtrX = urBounds.CenterX();
					PCOORD urBoundsCtrY = urBounds.CenterY();

					// get the relative component sizes and scale the unrotated bounds accordingly
					YComponentBoundingRect rOldBoundingRect = pOldComponentView->GetBoundingRect();
					YComponentBoundingRect rNewBoundingRect = pNewComponentView->GetBoundingRect();
					urBoundsW = (PCOORD)((YFloatType)urBoundsW * (rNewBoundingRect.Width()  / rOldBoundingRect.Width()) );
					urBoundsH = (PCOORD)((YFloatType)urBoundsH * (rNewBoundingRect.Height() / rOldBoundingRect.Height()) );

					// center the new bounds at the center of the old bounds
					urBounds.x0 = urBoundsCtrX - (urBoundsW / 2);
					urBounds.x1 = urBounds.x0 + urBoundsW;
					urBounds.y0 = urBoundsCtrY - (urBoundsH / 2);
					urBounds.y1 = urBounds.y0 + urBoundsH;;

					// set the new bounds
					set_unrotated_bound(urBounds);
					pRecord->release();
				}
			}
//			else
//			{
//				pNewComponentView->CopyBoundingRect( pOldComponentView );
//			}

			calc();

			// release our reference to the old one.
			((PMGDatabase*) database)->free_component_record( comp_record.recno );
		}

		comp_record.recno = record->Id();
		record->release();
	}

	return error;
}


// ****************************************************************************
//
//  Function Name:	SetDefaultSize( )
//
//  Description:		Sets the default size of this component slab
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void ComponentObject::SetDefaultSize( PBOX_PTR pPanelSize )
{
	try
	{
		RComponentView* pComponentView = GetComponentView();
		RComponentDocument* pComponentDoc = pComponentView->GetComponentDocument();

		try
		{

			// NOTE: the components default handler will throw an exeception
			// because there is no parent document.  If an execption is thrown,
			// then we will determine the component's size via the RObjectSize
			// class.  But by trying the component's method first, we can support
			// an overridden implementation.
			pComponentDoc->ResetToDefaultSize( NULL );
			return;
		}
		catch (...)
		{
		}


		RRealSize rSize = GetDefaultSize( pPanelSize );
		pComponentView->SetBoundingRect( RRealRect( rSize ) );
	}
	catch (...)
	{
	}
}

YComponentType ComponentObject::GetComponentType()
{
	RComponentDocument* pComponentDoc = GetComponentView()->GetComponentDocument();

	return pComponentDoc->GetComponentType();
}

RComponentView* ComponentObject::GetComponentView()
{
	ComponentRecord* pRecord = LockComponentRecord();
	RComponentView* pComponentView = pRecord->GetComponentView();
	pRecord->release();

	return pComponentView;
}

SHORT ComponentObject::GetContextMenuIndex()
{
	YComponentType type = GetComponentType();

	void* pValue = (SHORT*) 0L;
	_cContextMenuMap.Lookup( type, pValue );

	return (SHORT) pValue;
}

ERRORCODE ComponentObject::PasteComponent( COleDataObject* pDataObject, CPmwDoc* pDoc, PPNT& pastePoint )
{
	// Create an MFC data source
	RMFCDataTransferSource dataSource( pDataObject );

	// Create a component collection
	RComponentCollection componentCollection;

	// Get the component data
	YDataLength dataLength;
	uBYTE* pData = NULL;
	dataSource.GetData( kComponentFormat, pData, dataLength );

	// Make a storage out of this memory
	RChunkStorage storage( dataLength, pData );

	// Iterate through the components, checking their types
	RChunkStorage::RChunkSearcher searcher = storage.Start( kComponent, FALSE );
	BOOLEAN fUnsupportedComponentFound = FALSE;

	for( ;!searcher.End( ); ++searcher )
		{
		// Read in the component type
		YComponentType componentType;										
		storage >> componentType;

		// Check to see if its supported
		if( !::GetComponentManager( ).IsComponentAvailable( componentType ) ||
				(componentType == kHorizontalLineComponent ||
				componentType == kVerticalLineComponent ||
				componentType == kBorderComponent ||
				componentType == kMiniBorderComponent) )
			{
				fUnsupportedComponentFound = TRUE;
				break;
			}
		}

	delete [] pData;

	// If an unsupported component type was found, we have to pull down metafile format instead
	if( fUnsupportedComponentFound )
		componentCollection.PasteDataIntoComponent( dataSource, NULL, kEnhancedMetafileFormat );

	// If we didnt have to get a metafile, we can get the components
	if( componentCollection.Count( ) == 0 )
		componentCollection.Paste( dataSource, NULL );

	// Determine transformation matrix 
	// for converting logical units.
	R2dTransform transform;
	transform.PostScale( (float) PAGE_RESOLUTION / kSystemDPI, (float) PAGE_RESOLUTION / kSystemDPI );

	// Get the center point of the collection in PM logical units
	RRealPoint centerPoint = componentCollection.GetBoundingRect( ).GetCenterPoint( ) * transform;

	// Calculate the offset that needs to be applied to move the collection to the paste point
	RRealSize offset( pastePoint.x - centerPoint.m_x, pastePoint.y - centerPoint.m_y );

	PMGDatabase* pDatabase = pDoc->get_database();

	// Create slabs from the components and add them to the component list
	YComponentIterator iterator = componentCollection.Start( );
	for( ; iterator != componentCollection.End( ); ++iterator )
	{
		////////////////////////////////////////////////////////////////////
		// Remove the rotation and flip values, storing them so that they //
		// can be reappplied to the new RectPageObject.                   //
		////////////////////////////////////////////////////////////////////

		RComponentView* pComponentView = static_cast<RComponentView*>( ( *iterator )->GetActiveView( ) );

		bool bHflip = FALSE;
		bool bVflip = FALSE;

		if( pComponentView->IsFlippedHorizontal() )
		{
			pComponentView->FlipHorizontal( pComponentView->GetBoundingRect( ).GetCenterPoint( ).m_x );
			bHflip = TRUE;
		}
		if( pComponentView->IsFlippedVertical() )
		{
			pComponentView->FlipVertical( pComponentView->GetBoundingRect( ).GetCenterPoint( ).m_y );
			bVflip = TRUE;
		}

		YAngle angle;
		RRealSize scale;
		pComponentView->GetBoundingRect( ).GetTransform( ).Decompose( angle, scale.m_dx, scale.m_dy );
		pComponentView->Rotate( pComponentView->GetBoundingRect( ).GetCenterPoint( ), -angle );
		
		///////////////////////////////////////////
		// Create the new component page object. //
		///////////////////////////////////////////

		// First determine the size and position in PM logical units
		RRealRect rBounds = pComponentView->GetBoundingRect( ).m_TransformedBoundingRect * transform;
//		rBounds.Offset( offset );

		PBOX box   = { rBounds.m_Left, rBounds.m_Top, rBounds.m_Right, rBounds.m_Bottom };
		PPNT point = { box.x0, box.y0 };

		// Create a new component object
		RComponentDocument* pComponentDoc = pComponentView->GetComponentDocument();
		RectPageObject* pNewComponent = (RectPageObject*) pDatabase->create_component_object( pComponentDoc );
		pDoc->PasteObject( pNewComponent, &box, &box, FALSE );

		if (pNewComponent)
		{
			// Set component attributes
			//
			pNewComponent->set_bound( box );
			pNewComponent->set_rotation( angle );
			
			if (bHflip)
				pNewComponent->add_flags( OBJECT_FLAG_xflipped );

			if (bVflip)
				pNewComponent->add_flags( OBJECT_FLAG_yflipped );

			pNewComponent->calc();
		}
	}

	return ERRORCODE_None;
}


/*	private static method to return YComponentType from GRAPHIC_TYPE
*/
YComponentType ComponentObject::GetYComponentType( GRAPHIC_TYPE graphic_type )
{
	switch (graphic_type)
	{
	case GRAPHIC_TYPE_PS_SQUARE:
		return kSquareGraphicComponent;
	case GRAPHIC_TYPE_PS_ROW:
		return kRowGraphicComponent;
	case GRAPHIC_TYPE_PS_COLUMN:
		return kColumnGraphicComponent;
	case GRAPHIC_TYPE_CGM:
		return kCGMGraphicComponent;
	}

	TRACE( "ComponentObject::GetYComponentType: Graphic type not found.\n" );
	ASSERT( 0 );

	return kSquareGraphicComponent; 
}


/*	private static method to return EGraphicType from GRAPHIC_TYPE
*/
RGraphicInterface::EGraphicType ComponentObject::GetEGraphicType( GRAPHIC_TYPE graphic_type )
{
	switch (graphic_type)
	{
	case GRAPHIC_TYPE_PS_SQUARE:
		return RGraphicInterface::kSquare;
	case GRAPHIC_TYPE_PS_ROW:
		return RGraphicInterface::kRow;
	case GRAPHIC_TYPE_PS_COLUMN:
		return RGraphicInterface::kColumn;
	case GRAPHIC_TYPE_CGM:
		return RGraphicInterface::kCGM;
	}

	TRACE( "ComponentObject::GetEGraphicType: Graphic type not found.\n" );
	ASSERT( 0 );

	return RGraphicInterface::kSquare; 
}

/*	
static method used to create a new component based on the specified type
*/
RComponentDocument* ComponentObject::CreateNewComponent( const YComponentType& type, YComponentBoundingRect rBoundingRect )
{
	RComponentAttributes rAttributes;
	RComponentDocument *pNewDoc = ::GetComponentManager( ).CreateNewComponent(
				type,
				NULL,
				NULL,
				rAttributes,
				rBoundingRect,
				FALSE );

	return pNewDoc;
}

/*	
static method used to create a new component object from an existing non-component object
*/
PMGPageObject* ComponentObject::CreateFromObject( PMGPageObject* pObject )
{
	CRect crObject( 0, 0, kSystemDPI * 2, kSystemDPI * 2 );
	ComponentObject* pNewObject = NULL;
	BOOLEAN fScale = FALSE;

	switch (pObject->type())
	{
		case OBJECT_TYPE_Graphic:
			{
				GraphicRecord* pGraphic = ((GraphicObject *) pObject)->LockGraphicRecord();
				ReadOnlyFile File;
	
				if (ERRORCODE_None != pGraphic->prep_storage_file(&File, FALSE))
					break;

				RComponentDocument* pDocument = CreateNewComponent( kImageComponent, RRealRect( crObject ) );

				if (pDocument)
				{
					RComponentView* pView = (RComponentView *) pDocument->GetActiveView();
					RImageInterface* pInterface = (RImageInterface *) pView->GetInterface( kImageInterfaceId );
					
					if (pInterface)
					{
						ST_DEV_POSITION length;
						File.length( &length );

						// Create a memory storage
						RBufferStream stream;
						BYTE* pBuffer = stream.GetBuffer( length );

						DWORD nAmountRead;
						File.huge_read( pBuffer, length, &nAmountRead );
						ASSERT( nAmountRead == length );

						RStorage rStorage( &stream );
						RMBCString rPathName( pGraphic->m_csName );
						YException hException;
						
						if (pInterface->Load( rStorage, length, hException, &rPathName ))
							create( OBJECT_TYPE_Component, pObject->get_database(), pDocument, (PageObject **) &pNewObject );

						delete pInterface;
					}

					if (!pNewObject)
						delete pDocument;
				}
			}

			break;

		case OBJECT_TYPE_Ellipse:
		case OBJECT_TYPE_Rectangle:
		case OBJECT_TYPE_DrawingObject:
		{
			COutlinePath Path;

			if (((DrawingObject *) pObject)->GetPath( Path, crObject ))
			{
				RComponentDocument* pDocument = CreateNewComponent( kPathComponent, RRealRect( crObject ) );

				if (pDocument)
				{
					RPath rPath;
					R2dTransform transform;

					if (OBJECT_TYPE_DrawingObject != pObject->type())
					{
						// Ellipses and rectangles have fixed data points.
						// so scale for those here.
						transform.PostScale( 1.0 / 0xFFFF, 1.0 / 0xFFFF );
					}

					Path.GetRPath( rPath, transform );

					RComponentView* pView = (RComponentView *) pDocument->GetActiveView();
					IPathInterface* pInterface = (IPathInterface *) pView->GetInterface( kPathInterfaceId );
					
					if (pInterface)
					{
						pInterface->SetPath( rPath );
						delete pInterface;

						create( OBJECT_TYPE_Component, pObject->get_database(), pDocument, (PageObject **) &pNewObject );
					}

					if (!pNewObject)
						delete pDocument;
				}
			}

			break;
		}
	}

	if (pNewObject)
	{
		pNewObject->CopyAttributes( pObject );
		pNewObject->calc();
	}

	return pNewObject;
}

/*	static method used to create a new graphic document component based on the
	graphic creation structure provided.
*/
RComponentDocument *ComponentObject::CreateGraphicDocument(
	GRAPHIC_CREATE_STRUCT *pGcs, GRAPHIC_TYPE graphic_type )
{
	RComponentAttributes rAttributes;
	YComponentBoundingRect rBoundingRect(0,0,1,1);
	RComponentDocument *pNewDoc = ::GetComponentManager( ).CreateNewComponent(
				GetYComponentType(graphic_type),
				NULL,
				NULL,
				rAttributes,
				rBoundingRect,
				FALSE );

	// Check to see that we were able to create the new component.
	if (pNewDoc != NULL)
	{
		// Get the view and request an interface so we can have the component load the image.
		RComponentView * pGraphicView = static_cast<RComponentView*>(pNewDoc->GetActiveView());
		RGraphicInterface * pGraphicInterface = dynamic_cast<RGraphicInterface*>(pGraphicView->GetInterface(kGraphicInterfaceId));

		if (pGraphicInterface != NULL)
		{
			// Set the path from the GCS.
			CString strCollectionName;
			CString strFileName;

			PathBindingType type = GetGlobalPathManager()->BindPath( pGcs->m_csFileName, strFileName, &strCollectionName );
			if (type == PBT_CollectionItem)
			{
				// We are dealing with a collection item, so attempt to load it.
				// The file is being dynamically allocated so that we can delete it
				// before putting the bits into the component.  This avoids causing
				// us to have 3 copies of the raw bits in memory simultaneously.
				StorageFile *pfileImage = new StorageFile( pGcs->m_csFileName );

				// Find out how big it is so we will know how much data to read.
				ST_DEV_POSITION lSize;
				ERRORCODE eOpen = pfileImage->length( &lSize );	// This should open and read in the file.

				// Check to see that we opened the file OK and retrieved the size.
				if (eOpen != ERRORCODE_None)
				{
					delete pfileImage;
					TRACE( "Failure to open the image file &s: It may have been on another volume and the user canceled.\n", pGcs->m_csFileName );
					return NULL;
				}

				// Allocate a data buffer to hold the image data to transfer over to the component.
				char *pRawData = new char[lSize];
				DWORD dwSizeRead;
				pfileImage->huge_read( pRawData, lSize, &dwSizeRead );
				delete pfileImage;

				// Verify for completeness that we got the entire file.
				if (dwSizeRead != (DWORD)lSize)
				{
					TRACE( "Failure to read in the raw image data buffer for transfer to the component.\n" );
					ASSERT( 0 );
					delete [] pRawData;
					delete pNewDoc;
					return NULL;
				}

				// We have a data buffer storing the raw image data, now get it into the component.
				RBufferStream streamRawData( (unsigned char *)pRawData, lSize );
				RStorage fileMem( &streamRawData );
				if (graphic_type == GRAPHIC_TYPE_CGM)
				{
					pGraphicInterface->Load( fileMem, lSize, GetEGraphicType(graphic_type) );
				}
				else
				{
					pGraphicInterface->LoadFromPretzelCollection( fileMem, GetEGraphicType(graphic_type) );
				}

				// Clean up the temporary memory buffer.
				delete [] pRawData;
			}
			else
			{
				// We are importing the image, so we can just load off of the filename as it is.
				CString strFixedPath = GetGlobalPathManager()->LocatePath( pGcs->m_csFileName );
			
				RMBCString rPath = RMBCString((LPCSTR)strFixedPath);
				RStorage storage( rPath, kRead );

				if (graphic_type == GRAPHIC_TYPE_CGM)
				{
					YStreamLength nLength = storage.GetStorageStream()->GetSize();
					pGraphicInterface->Load(storage, nLength, GetEGraphicType(graphic_type));
				}
				else
				{
					pGraphicInterface->LoadFromPretzelCollection(storage, GetEGraphicType(graphic_type));
				}
			}
		}
		else
		{
			delete pNewDoc;
			pNewDoc = NULL;
		}

		// delete the interface
		delete pGraphicInterface;
	}

	return pNewDoc;
}


/*	static method used to create a new image document component based on the
	graphic creation structure provided.
*/
RComponentDocument *ComponentObject::CreateImageDocument(
	GRAPHIC_CREATE_STRUCT *pGcs )	// A pointer to the data describing the object to create.
{
	RComponentAttributes rAttributes;
	YComponentBoundingRect rBoundingRect(0,0,1,1);
	RComponentDocument *pNewDoc = ::GetComponentManager( ).CreateNewComponent(
				kImageComponent,
				NULL,
				NULL,
				rAttributes,
				rBoundingRect,
				FALSE );

	// Check to see that we were able to create the new component.
	if (pNewDoc != NULL)
	{
		// Get the view and request an interface so we can have the component load the image.
		RComponentView * pImageView = static_cast<RComponentView*>(pNewDoc->GetActiveView());
		RImageInterface * pImageInterface = dynamic_cast<RImageInterface*>(pImageView->GetInterface(kImageInterfaceId));

		if (pImageInterface != NULL)
		{
			// Set the path from the GCS.
			CString strCollectionName;
			CString strFileName;

			PathBindingType type = GetGlobalPathManager()->BindPath( pGcs->m_csFileName, strFileName, &strCollectionName );
			if (type == PBT_CollectionItem)
			{
				// We are dealing with a collection item, so attempt to load it.
				// The file is being dynamically allocated so that we can delete it
				// before putting the bits into the component.  This avoids causing
				// us to have 3 copies of the raw bits in memory simultaneously.
				StorageFile *pfileImage = new StorageFile( pGcs->m_csFileName );

				// Find out how big it is so we will know how much data to read.
				ST_DEV_POSITION lSize;
				ERRORCODE eOpen = pfileImage->length( &lSize );	// This should open and read in the file.

				// Check to see that we opened the file OK and retrieved the size.
				if (eOpen != ERRORCODE_None)
				{
					delete pfileImage;
					TRACE( "Failure to open the image file &s: It may have been on another volume and the user canceled.\n", pGcs->m_csFileName );
					return NULL;
				}

				// Allocate a data buffer to hold the image data to transfer over to the component.
				char *pRawData = new char[lSize];
				DWORD dwSizeRead;
				pfileImage->huge_read( pRawData, lSize, &dwSizeRead );
				delete pfileImage;

				// Verify for completeness that we got the entire file.
				if (dwSizeRead != (DWORD)lSize)
				{
					TRACE( "Failure to read in the raw image data buffer for transfer to the component.\n" );
					ASSERT( 0 );
					delete [] pRawData;
					delete pNewDoc;
					return NULL;
				}

				// We have a data buffer storing the raw image data, now get it into the component.
				RBufferStream streamRawData( (unsigned char *)pRawData, lSize );
				RStorage fileMem( &streamRawData );
				YException hException;
				pImageInterface->Load( fileMem, lSize, hException );

				// Clean up the temporary memory buffer.
				delete [] pRawData;
			}
			else
			{
				// We are importing the image, so we can just load off of the filename as it is.
				CString strFixedPath = GetGlobalPathManager()->LocatePath( pGcs->m_csFileName );
				
				YException hException;
				RMBCString rPath = RMBCString((LPCSTR)strFixedPath);
				if (!pImageInterface->Load(rPath, hException))
				{
					delete pNewDoc;
					pNewDoc = NULL;
				}
			}
		}
		else
		{
			delete pNewDoc;
			pNewDoc = NULL;
		}

		// delete the interface
		delete pImageInterface;
	}

	return pNewDoc;
}

/*
// Add the outline for this object to the passed array.
//
// Gets the YAvoidancePathCollection from the component and coverts the
// points to a COutlinePath. This function is called when calculating the
// avoidance path of text around an object.
//
// If pClipBox is not NULL, it is assumed that an intersect test has
// already been performed with this box against the object bound.
*/

void ComponentObject::AddOutline(COutlinePath& Path, PBOX* pClipBox /*=NULL*/)
{

#ifdef _DEBUG
   PBOX Bound = get_unrotated_bound();
   CRect r;
   r.left = PageToInches(Bound.x0);
   r.top = PageToInches(Bound.y0);
   r.right = PageToInches(Bound.x1);
   r.bottom = PageToInches(Bound.y1);
	TRACE("Bounding Rect = left %d, top %d, right %d, bottom %d\n", r.left, r.top, r.right, r.bottom );
#endif

	// if non-irregular text flow call base class method
	if( GetFlowAround() == FLOW_Box )
		RectPageObject::AddOutline( Path, pClipBox );
	else if( GetFlowAround() == FLOW_Shape )
	{
		RComponentView* pView = GetComponentView();

		if( pView )
		{
			// Get the avoidance path from the component
			YAvoidancePathCollection pathCollection;
			pView->AddOutline( pathCollection, R2dTransform(), 0 );

			// Make sure we have one and only one path.
			TpsAssert( pathCollection.Count( ) == 1, "Must have one and only one path." );

			RIntPoint* pPoints = NULL;
			YPointCountArray countArray;
			( *pathCollection.Start( ) )->CalcPath( R2dTransform( ), pPoints, countArray );

			// get the bounds of the object for the x,y offset form the page
			PBOX Bound = get_unrotated_bound();

			// iterate through the collection of points and convert each path to a COutlinePath
			RIntPoint* p = pPoints;

			YPointCountArray::YIterator iterator = countArray.Start( );
			for( ; iterator != countArray.End( ); ++iterator )
			{	
				COutlinePath LocalPath;
				int newx, newy, lastx, lasty, firstx, firsty;
				newx, newy, lastx, lasty = 0;
				int nPointsAdded = 0;
				// Add the points
				TRACE("New Path\n");
				for( int i = 0; i < ( *iterator ); ++i )
				{
					// convert from component units to app units, could do in a transform
					newx = PageToInches( Bound.x0 + MulDiv( p->m_x, PAGE_RESOLUTION, kSystemDPI ) );
					newy = PageToInches( Bound.y0 + MulDiv( p->m_y, PAGE_RESOLUTION, kSystemDPI ) );
					if( i == 0 )
					{
						LocalPath.MoveTo(	newx, newy );
						firstx = lastx = newx;
						firsty = lasty = newy;
						TRACE("Points added x = %d,	y = %d\n", newx, newy );
//						ASSERT( r.PtInRect( CPoint( newx, newy  ) ) );
					}
					else if(!(newx == lastx && newy == lasty ) ) // don't allow duplicate points
					{
						LocalPath.LineTo( newx , newy );
						nPointsAdded++;
						TRACE("Points added x = %d,	y = %d\n", newx, newy );
//						ASSERT( r.PtInRect( CPoint( newx, newy) ) );
						lastx = newx;
						lasty = newy;
					}
					p++;
				}
				if( nPointsAdded > 0 )
				{
					// End the path
					LocalPath.Close();
					LocalPath.End();

					TRACE("Added %d points\n\n", ++nPointsAdded );
					// Add to the path passed
					Path.Append(&LocalPath);
				}
			}
			// Clean up
			::ReleaseGlobalBuffer( reinterpret_cast<uBYTE*>(pPoints) );

			YAvoidancePathCollection::YIterator pathIterator = pathCollection.Start( );
			for( ; pathIterator != pathCollection.End( ); ++pathIterator )
			{
				delete (*pathIterator);
			}
		}
	}
 }



// Determine if the current stored component represents an animated GIF image.
bool ComponentObject::IsAnimatedGIF()
{
	bool bIsAnimGIF = FALSE;

	if (GetComponentType() == kImageComponent)
	{
		RComponentView *pCompView = GetComponentView();
		if (pCompView != NULL)
		{
			RImageTypeInterface * pImageTypeInterface = dynamic_cast<RImageTypeInterface*>(pCompView->GetInterface(kImageTypeInterfaceId));

			if (pImageTypeInterface != NULL)
			{
				// Check the document ot see if it contains an animated GIF.
				bIsAnimGIF = (pImageTypeInterface->QueryImageIsAnAnimatedGIF() != FALSE);
			}

			delete pImageTypeInterface;
		}
	}

	return bIsAnimGIF;
}


// If the current stored component represents an animated GIF, create a GIF
// animator for it and return it so that the caller can perform the GIF
// animation preview.
//
// Returns : A pointer to the allocated gif animator.  It is the callers
//				 responsibility to deallocate the memory associated with this object.
//				 The pointer is NULL if a problem occurs.
CPGIFAnimator *ComponentObject::CreateGIFAnimator()
{
	CPGIFAnimator *pGifAnimator = NULL;
	if (GetComponentType() == kImageComponent)
	{
		RComponentView *pCompView = GetComponentView();
		if (pCompView != NULL)
		{
			RImageTypeInterface * pImageTypeInterface = dynamic_cast<RImageTypeInterface*>(pCompView->GetInterface(kImageTypeInterfaceId));

			if (pImageTypeInterface != NULL)
			{
				// Build a GIF animator and attach it to a GIF Reader retrieved form the document.
				pGifAnimator = pImageTypeInterface->CreateGIFAnimator();
			}

			delete pImageTypeInterface;
		}
	}

	return pGifAnimator;
}


// Checks to see if the component stores raw GIF or JPEG data.
//
// Returns : TRUE if the data is in either GIF or JPEG format, FALSE if not.
bool ComponentObject::IsWebImageFormat()
{
	bool bIsWebFormat = FALSE;

	if (GetComponentType() == kImageComponent)
	{
		RComponentView *pCompView = GetComponentView();
		if (pCompView != NULL)
		{
			RImageTypeInterface * pImageTypeInterface = dynamic_cast<RImageTypeInterface*>(pCompView->GetInterface(kImageTypeInterfaceId));

			if (pImageTypeInterface != NULL)
			{
				bIsWebFormat = (pImageTypeInterface->QueryIsWebImageFormat() != FALSE);
			}

			delete pImageTypeInterface;
		}
	}

	return bIsWebFormat;
}


// Checks to see if the component stores raw JPEG data.
//
// Returns : TRUE if the data is in JPEG format, FALSE if not.
bool ComponentObject::IsJPEGImage()
{
	bool bIsJPEGFormat = FALSE;

	if (GetComponentType() == kImageComponent)
	{
		RComponentView *pCompView = GetComponentView();
		if (pCompView != NULL)
		{
			RImageTypeInterface * pImageTypeInterface = dynamic_cast<RImageTypeInterface*>(pCompView->GetInterface(kImageTypeInterfaceId));

			if (pImageTypeInterface != NULL)
			{
				EImageFormat eFormat = pImageTypeInterface->QueryImageFormat();
				bIsJPEGFormat = (eFormat == kImageFormatJPG);
			}

			delete pImageTypeInterface;
		}
	}

	return bIsJPEGFormat;
}


// Attempts to export the raw data in the component into the specified file
// name.  This will only be successfull if the raw data is in JPEG or GIF format
// at this time.  This method also assumes that the supplied filename does not
// already include an extension.  The extension is added here.
//
// The filename has the correct extension concatenated onto it.
//
// Returns : TRUE if the data can be exported in JPEG or GIF format, FALSE if not.
bool ComponentObject::ExportRawData( CString *pstrFileName )
{
	// Ensure we are given a legitimate filename buffer.  Return if it is NULL.
	ASSERT( pstrFileName );
	if (pstrFileName == NULL)
	{
		return FALSE;
	}
	ASSERT( !pstrFileName->IsEmpty() );
	if (pstrFileName->IsEmpty())
	{
		return FALSE;
	}

	bool bSucceeded = FALSE;

	if (GetComponentType() == kImageComponent)
	{
		RComponentView *pCompView = GetComponentView();
		if (pCompView != NULL)
		{
			RImageTypeInterface * pImageTypeInterface = dynamic_cast<RImageTypeInterface*>(pCompView->GetInterface(kImageTypeInterfaceId));

			if (pImageTypeInterface != NULL)
			{
				EImageFormat eFormat = pImageTypeInterface->QueryImageFormat();

				// Export the raw data.
				RImageInterface * pImageInterface = dynamic_cast<RImageInterface*>(pCompView->GetInterface(kImageInterfaceId));
				if (pImageInterface != NULL)
				{
					RMBCString rstrFile = *pstrFileName;
					bSucceeded = (pImageInterface->Export(rstrFile, eFormat) != FALSE);
					delete pImageInterface;
				}
			}

			delete pImageTypeInterface;
		}
	}

	return bSucceeded;
}


// Provides the size of the image contained in the raw data if it can be
// determined.  This is only possible for image components.  All other
// components will leave the size unchanged.
void ComponentObject::GetRawImageSize( CSize *psizeImage )
{
	// Ensure we have somewhere to put the raw image size.
	ASSERT( psizeImage );
	if (psizeImage == NULL)
	{
		return;
	}

	// Get the raw image size if we can.
	if (GetComponentType() == kImageComponent)
	{
		RComponentView *pCompView = GetComponentView();
		if (pCompView != NULL)
		{
			RImageInterface * pImageInterface = dynamic_cast<RImageInterface*>(pCompView->GetInterface(kImageInterfaceId));

			if (pImageInterface != NULL)
			{
				RIntSize rsizeImage(psizeImage->cx, psizeImage->cy);
				pImageInterface->QueryRawImageSize(&rsizeImage);
				psizeImage->cx = rsizeImage.m_dx;
				psizeImage->cy = rsizeImage.m_dy;
			}

			delete pImageInterface;
		}
	}
}



/* Detaches and returns the document attached to the component record.

	NULL if the process fails or the record is not linked to a document.
*/
RComponentDocument* ComponentObject::DetachComponentDocument()
{
	ComponentRecord* pComponent = LockComponentRecord();
	if (!pComponent)
		return NULL;

	RComponentDocument *pDetachedDoc = pComponent->DetachComponentDocument();

	pComponent->release();

	return pDetachedDoc;
}



/* Retrieves the original path of the image stored for the graphic.
 */
const CString *ComponentObject::GetOriginalDataPath()
{
	// We use a static here to allow us to return something that won't vanish.
	// This will mean that we can't return NULL on failure, so make sure it is empty.
	static CString strFileName;
	strFileName.Empty();

	// We must be an image component for a path to be stored.
	if (GetComponentType() == kImageComponent)
	{
		RComponentView *pCompView = GetComponentView();
		if (pCompView != NULL)
		{
			RImageInterface * pImageInterface = dynamic_cast<RImageInterface*>(pCompView->GetInterface(kImageInterfaceId));

			if (pImageInterface != NULL)
			{
				strFileName = (CString)pImageInterface->GetPath();
			}

			delete pImageInterface;
		}
	}

	return &strFileName;
}
