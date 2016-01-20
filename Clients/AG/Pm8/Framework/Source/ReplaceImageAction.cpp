// ****************************************************************************
//
//  File Name:			ReplaceImageAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				John Fleischhauer
//
//  Description:		Declaration of the RReplaceImageAction class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc.
//
//  Copyright (C) 1998 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "ReplaceImageAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "FrameworkResourceIds.h"
#include "CompositeSelection.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "ComponentTypes.h"
#include "ImageInterface.h"
#include "BitmapImage.h"
#include "Path.h"


YActionId RReplaceImageAction::m_ActionId( "RReplaceImageAction" );

// ****************************************************************************
//
//  Function Name:	RReplaceImageAction::RReplaceImageAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RReplaceImageAction::RReplaceImageAction( RCompositeSelection* pCurrentSelection,
														RComponentDocument* pOldComponent,
														RComponentDocument* pNewComponent,
														YResourceId undoRedoPairId )
	: RUndoableAction( m_ActionId, undoRedoPairId, undoRedoPairId + 1 ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_pComponent( pOldComponent ),
	  m_pReplacementImage( NULL ),
	  m_pReplacementClippingPath( NULL ),
	  m_pOldImage( NULL ),
	  m_pOldClippingPath( NULL ),
	  m_pFramePath( NULL ),
	  m_bMustCalculateBoundingRect( FALSE )
{
	// first get some stuff from the OLD image
	RComponentView* pComponentView = static_cast<RComponentView*>( pNewComponent->GetActiveView( ) );
	RImageInterface* pImageInterface = dynamic_cast<RImageInterface*>(pComponentView->GetInterface(kImageInterfaceId));

	// get a copy of the new image
	m_pReplacementImage = static_cast<RBitmapImage *>( pImageInterface->CopySourceImage() );

	// and the new clipping path
	REditImageInterface* pEditImageInterface = static_cast<REditImageInterface*>(pComponentView->GetInterface( kEditImageInterfaceId ));
	RClippingPath rPath;
	pEditImageInterface->GetClippingPath( rPath );
	if ( rPath.IsDefined() )
	{
		m_pReplacementClippingPath = new RClippingPath( rPath );
	}

	// get the new crop area
	RImageEffects rEffects = pEditImageInterface->GetImageEffects();
	RRealRect rNewCropArea = rEffects.m_rCropArea;

	delete pImageInterface;
	delete pEditImageInterface;

	// ok - now get some stuff from the OLD image
	pComponentView = static_cast<RComponentView*>( pOldComponent->GetActiveView( ) );
	pImageInterface = dynamic_cast<RImageInterface*>(pComponentView->GetInterface(kImageInterfaceId));

	// get a copy of the old, to-be-replaced image
	m_pOldImage = static_cast<RBitmapImage *>( pImageInterface->CopySourceImage() );

	// get the old bounding rect
	m_rOldBoundingRect = pComponentView->GetBoundingRect();
	
	// for now, we will assume that the bounding rect will be maintained
	m_rReplacementBoundingRect = m_rOldBoundingRect;

	// get the old clipping path, if any
	pEditImageInterface = static_cast<REditImageInterface*>(pComponentView->GetInterface( kEditImageInterfaceId ));
	pEditImageInterface->GetClippingPath( rPath );

	// and save it
	if ( rPath.IsDefined() )
	{
		m_pOldClippingPath = new RClippingPath( rPath );
	}

	// we will want to retain the frame path, too
	//
	// *NOTE*:  This action *assumes* that the size or orientation of an image
	//				with a frame path has not been altered (e.g., the Rotate 90 tool
	//				in the Photo Workshop is disabled for such images). Here is a
	//				small safety check, but it will not catch the case of a rotated
	//				square image.
	//
	//				If you alter the size or orientation of the image with a frame
	//				path, that path will get tossed, and will not be recovered in the
	//				Undo.  Bottom line caveat: you should not allow such alterations
	//				on images with frame paths.
	//
	RRealSize rOldImageSize = m_pOldImage->GetSizeInLogicalUnits();
	RRealSize rNewImageSize = m_pReplacementImage->GetSizeInLogicalUnits();
	if ( rOldImageSize == rNewImageSize )
	{
		RBitmapImage* pOldBitmap = dynamic_cast<RBitmapImage*>( pImageInterface->GetImage() );
		if ( pOldBitmap )
		{
			RPath* pFramePath = pOldBitmap->GetFrameRPath();
			if (pFramePath )
			{
				m_pFramePath = new RPath( *pFramePath );
			}
		}
	}

	// get the old crop area
	rEffects = pEditImageInterface->GetImageEffects();
	RRealRect rOldCropArea = rEffects.m_rCropArea;

	delete pImageInterface;
	delete pEditImageInterface;

	// if the image and existing crop were not rotated, or if they were rotated
	//	180-degrees, then we can use the old bounding rect
	if ( ! Rotated0or180( rOldImageSize, rNewImageSize, rOldCropArea, rNewCropArea ) )
	{
		// otherwise, see if we have a 90-degree or 270-degree roation of an image
		//	with an existing crop
		if ( Rotated90or270( rOldImageSize, rNewImageSize, rOldCropArea, rNewCropArea ) )
		{
			// in that case, we can swap the bounding rect dimensions as well
			RRealPoint rOldCenterPoint( m_rReplacementBoundingRect.GetCenterPoint() );
			R2dTransform rTransform = m_rReplacementBoundingRect.GetTransform();

			// remember the critical transform elements
			YAngle rAngle;
			YRealDimension xScale, yScale;
			rTransform.Decompose( rAngle, xScale, yScale );

			// untransform the bounding rect and center it at 0,0
			rTransform.Invert();
			m_rReplacementBoundingRect *= rTransform;

			RRealPoint rTempCenterPoint( m_rReplacementBoundingRect.GetCenterPoint() );
			m_rReplacementBoundingRect.Offset( RRealSize( -rTempCenterPoint.m_x, -rTempCenterPoint.m_y ) );

			// swap the dimensions
			m_rReplacementBoundingRect.Set(	m_rReplacementBoundingRect.m_TopLeft.m_y,
														m_rReplacementBoundingRect.m_TopLeft.m_x,
														m_rReplacementBoundingRect.m_BottomRight.m_y,
														m_rReplacementBoundingRect.m_BottomRight.m_x );

			// now rescale and rotate the rect back to its initial factors
			rTempCenterPoint = m_rReplacementBoundingRect.GetCenterPoint();

			m_rReplacementBoundingRect.ScaleAboutPoint( rTempCenterPoint, RRealSize( yScale, xScale ) );
			m_rReplacementBoundingRect.RotateAboutPoint( rTempCenterPoint, rAngle );

			// finally, center it back on the old center point
			m_rReplacementBoundingRect.Offset( RRealSize( rOldCenterPoint.m_x, rOldCenterPoint.m_y ) );

			// sanity check
			rTempCenterPoint = m_rReplacementBoundingRect.GetCenterPoint();
		}
		else
		{
			// if not, then we have a new crop, so we must determine the bounding rect in Do()
			m_bMustCalculateBoundingRect = TRUE;
		}
	}

	// we got what we need; we don't need this any more
	delete pNewComponent;
}

// ****************************************************************************
//
//  Function Name:	RReplaceImageAction::~RReplaceImageAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RReplaceImageAction::~RReplaceImageAction( )
{
	delete m_pReplacementImage;
	delete m_pReplacementClippingPath;

	delete m_pOldImage;
	delete m_pOldClippingPath;
	delete m_pFramePath;
}

// ****************************************************************************
//
//  Function Name:	RReplaceImageAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RReplaceImageAction::Do( )
{
	RWaitCursor rCursor;
	
	// call the base method to setup the state properly
	RUndoableAction::Do( );

	// unselect everything
	m_pCurrentSelection->Invalidate( );
	m_pCurrentSelection->UnselectAll( FALSE );

	RComponentView* pComponentView = static_cast<RComponentView*>( m_pComponent->GetActiveView( ) );
	RImageInterface* pImageInterface = dynamic_cast<RImageInterface*>(pComponentView->GetInterface(kImageInterfaceId));

	//	if this flag is set, it's because the crop area has changed, so we must calculate what
	//	the bounding rect of the entire uncropped image would be
	if ( m_bMustCalculateBoundingRect )
	{
		// determine original scaling and rotation factors for the old bounding rect / old image size
		m_rReplacementBoundingRect = m_rOldBoundingRect;

		YAngle rAngle;
		RRealSize scale;
		m_rReplacementBoundingRect.GetTransform().Decompose( rAngle, scale.m_dx, scale.m_dy );

/*		RRealSize flipScale( 1, 1 );

		if (scale.m_dx < 0)
		{
			flipScale.m_dx = -1;
			scale.m_dx = -scale.m_dx;
		}
		if (scale.m_dy < 0)
		{
			flipScale.m_dy = -1;
			scale.m_dy = -scale.m_dy;
		}
*/
		RRealSize rImageSize = m_pOldImage->GetSizeInLogicalUnits();
		REditImageInterface* pEditImageInterface = static_cast<REditImageInterface*>(pComponentView->GetInterface( kEditImageInterfaceId ));
		const RImageEffects& rEffects = pEditImageInterface->GetImageEffects();
		delete pEditImageInterface;

		RRealSize rSize = m_rReplacementBoundingRect.GetUntransformedSize();
//		scale.m_dx *= rSize.m_dx / (rImageSize.m_dx * rEffects.m_rCropArea.m_Right);
//		scale.m_dy *= rSize.m_dy / (rImageSize.m_dy * rEffects.m_rCropArea.m_Bottom);

		RRealSize rUntransformedScale(
			rSize.m_dx / (rImageSize.m_dx * rEffects.m_rCropArea.m_Right),
			rSize.m_dy / (rImageSize.m_dy * rEffects.m_rCropArea.m_Bottom) );

		// set the new image into the component
		pImageInterface->SetImage( *m_pReplacementImage, TRUE, TRUE );

		// re-scale the component's new bounding rect by it's original scaling factor
		m_rReplacementBoundingRect = pComponentView->GetBoundingRect();

		// Untransformed scale
		m_rReplacementBoundingRect.UnrotateAndResizeAboutPoint( 
			m_rReplacementBoundingRect.GetCenterPoint(), rUntransformedScale );

		// Transformed scale
//		m_rReplacementBoundingRect.UnrotateAndScaleAboutPoint( 
//			m_rReplacementBoundingRect.GetCenterPoint(), scale );

		// if a previous crop had been resized, so that removing that crop or recropping
		//	the image now causes it to be humungous, keep it inside the panel
		if ( pComponentView->GetParentView() )
		{
			// compare our calculated size to the panel size
			RRealSize rPanelSize = pComponentView->GetParentView()->GetSize();
			if (	m_rReplacementBoundingRect.Width()  > rPanelSize.m_dx ||
					m_rReplacementBoundingRect.Height() > rPanelSize.m_dy )
			{
				RRealRect rPanelRect( rPanelSize );
				RRealRect rBoundsRect( m_rReplacementBoundingRect.WidthHeight() );

				YScaleFactor rPanelScale = rBoundsRect.ShrinkToFit( rPanelRect );
				m_rReplacementBoundingRect.ScaleAboutPoint( m_rReplacementBoundingRect.GetCenterPoint(), RRealSize(rPanelScale, rPanelScale) );
			}
		}

		// ok, we've got our bounding rect
		pComponentView->SetBoundingRect( m_rReplacementBoundingRect );
//		pComponentView->Scale( RRealPoint( 0, 0 ), flipScale, FALSE );
		
		// we should only have to do this once
		m_bMustCalculateBoundingRect = FALSE;
	}
	else
	{
		// just set the new image and reset the bounding rect
		pImageInterface->SetImage( *m_pReplacementImage, TRUE, TRUE );
		pComponentView->SetBoundingRect( m_rReplacementBoundingRect );
	}

	// restore the frame path
	if ( m_pFramePath )
	{
		RBitmapImage* pNewBitmap = dynamic_cast<RBitmapImage*>( pImageInterface->GetImage() );
		pNewBitmap->SetFrameRPath( m_pFramePath );
	}

	delete pImageInterface;

	// select the new component
 	m_pCurrentSelection->Select( pComponentView, FALSE );
	m_pCurrentSelection->Invalidate( );

	//	tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );

	// switch the critical members for subsequent undo or redo
	RBitmapImage* pTempImage = m_pReplacementImage;
	m_pReplacementImage = m_pOldImage;
	m_pOldImage = pTempImage;

	RClippingPath* pTempPath = m_pReplacementClippingPath;
	m_pReplacementClippingPath = m_pOldClippingPath;
	m_pOldClippingPath = pTempPath;

	YComponentBoundingRect rTempRect = m_rReplacementBoundingRect;
	m_rReplacementBoundingRect = m_rOldBoundingRect;
	m_rOldBoundingRect = rTempRect;

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RReplaceImageAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RReplaceImageAction::Undo( )
{
	// Call Do() to do the switch
	Do();

	// Call the base method to setup the state properly
	RUndoableAction::Undo();
}

// ****************************************************************************
//
//  Function Name:	RReplaceImageAction::Rotated0or180()
//
//  Description:		Reports if the new image / cropping area appears to be a
//							non-rotation, or a 180-degree rotation, of the old image
//							and crop
//
//  Returns:			TRUE if 0 or 180 degree rotation
//
//							Note: If the tests here prove true, it does not matter if it
//									was actually the result of a rotation or of some other
//									user edit.  As long as the ratios agree, we will get the
//									bounding rect we want for this Action.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RReplaceImageAction::Rotated0or180( RRealSize& rOldImageSize, RRealSize& rNewImageSize, RRealRect& rOldCropArea, RRealRect& rNewCropArea )
{
	BOOLEAN bRotated0or180 = FALSE;

	//	if the image dimensions are the same we may have a 0 / 180 rotation
	if ( rNewImageSize == rOldImageSize )
	{
		// if the crop areas are the same, that's all we need to know
		if ( rNewCropArea == rOldCropArea )
		{
			bRotated0or180 = TRUE;
		}
		else
		{
			// determine where the crop areas fall in the two images
			YRealDimension rWidth  = m_pOldImage->GetWidthInPixels();
			YRealDimension rHeight = m_pOldImage->GetHeightInPixels();

			YRealDimension rOldLeft   = rWidth  * rOldCropArea.m_Left;
			YRealDimension rOldTop    = rHeight * rOldCropArea.m_Top;
			YRealDimension rOldRight  = rOldLeft + (rWidth  * rOldCropArea.m_Right);
			YRealDimension rOldBottom = rOldTop  + (rHeight * rOldCropArea.m_Bottom);

			YRealDimension rNewLeft   = rWidth  * rNewCropArea.m_Left;
			YRealDimension rNewTop    = rHeight * rNewCropArea.m_Top;
			YRealDimension rNewRight  = rNewLeft + (rWidth  * rNewCropArea.m_Right);
			YRealDimension rNewBottom = rNewTop  + (rHeight * rNewCropArea.m_Bottom);

			// if rotated 180 degrees
			if (	( abs( (rWidth	 - rNewRight ) - rOldLeft ) <= 1 )	&&
					( abs( (rHeight - rNewBottom) - rOldTop  ) <= 1 )	)
			{
				bRotated0or180 = TRUE;
			}

		}
	}

	return bRotated0or180;
}

// ****************************************************************************
//
//  Function Name:	RReplaceImageAction::Rotated0or180()
//
//  Description:		Reports if the new image / cropping area appears to be a
//							90-degree rotation or a 270-degree rotation of the old
//							image and crop
//
//  Returns:			TRUE if 90 or 270 degree rotation
//
//							Note: If the tests here prove true, it does not matter if it
//									was actually the result of a rotation or of some other
//									user edit.  As long as the ratios agree, we will get the
//									bounding rect we want for this Action.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RReplaceImageAction::Rotated90or270( RRealSize& rOldImageSize, RRealSize& rNewImageSize, RRealRect& rOldCropArea, RRealRect& rNewCropArea )
{
	BOOLEAN bRotated90or270 = FALSE;

	//	if the image dimensions and the crop dimensions are both reversed,
	//	we may have a 90 / 270 rotation
	if (	rNewImageSize.m_dx == rOldImageSize.m_dy	&&
			rNewImageSize.m_dy == rOldImageSize.m_dx	&&
			rNewCropArea.m_Right  == rOldCropArea.m_Bottom	&&
			rNewCropArea.m_Bottom == rOldCropArea.m_Right	)
	{
		// determine where the crop areas fall in the two images
		YRealDimension rOldWidth  = m_pOldImage->GetWidthInPixels();
		YRealDimension rOldHeight = m_pOldImage->GetHeightInPixels();
		YRealDimension rNewWidth  = m_pReplacementImage->GetWidthInPixels();
		YRealDimension rNewHeight = m_pReplacementImage->GetHeightInPixels();

		YRealDimension rOldLeft   = rOldWidth  * rOldCropArea.m_Left;
		YRealDimension rOldTop    = rOldHeight * rOldCropArea.m_Top;
		YRealDimension rOldRight  = rOldLeft + (rOldWidth  * rOldCropArea.m_Right);
		YRealDimension rOldBottom = rOldTop  + (rOldHeight * rOldCropArea.m_Bottom);

		YRealDimension rNewLeft   = rNewWidth  * rNewCropArea.m_Left;
		YRealDimension rNewTop    = rNewHeight * rNewCropArea.m_Top;
		YRealDimension rNewRight  = rNewLeft + (rNewWidth  * rNewCropArea.m_Right);
		YRealDimension rNewBottom = rNewTop  + (rNewHeight * rNewCropArea.m_Bottom);

		// if rotated 90 degrees
		if (	(	( abs( (rNewWidth	- rNewRight  ) - rOldTop  ) <= 1 )	&&
					( abs(  rNewTop   - rOldLeft   ) <= 1 )					)	||
				// or 270 degrees
				(	( abs(  rNewLeft   - rOldTop   ) <= 1 )	&&
					( abs( (rNewHeight - rNewBottom) - rOldLeft ) <= 1 )	)	)
		{
			bRotated90or270 = TRUE;
		}
	}

	return bRotated90or270;
}

// ****************************************************************************
//
//  Function Name:	RReplaceImageAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RReplaceImageAction::WriteScript( RScript& /* script */) const
{
	return TRUE;
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RReplaceImageAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RReplaceImageAction::Validate( ) const
{
	RUndoableAction::Validate( );
	TpsAssertIsObject( RReplaceImageAction, this );
}

#endif	//	TPSDEBUG
