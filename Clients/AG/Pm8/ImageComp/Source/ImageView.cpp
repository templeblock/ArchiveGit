// ************************************************************************
//
//  File Name:			ImageView.cpp
//
//  Project:			Image Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RImageView class
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
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/ImageComp/Source/ImageView.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:21p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"ImageIncludes.h"

ASSERTNAME

#include	"ImageView.h"
#include	"ImageDocument.h"
#include "FrameworkResourceIDs.h"
#include "VectorImage.h"
#include "Menu.h"
#include "TintInterface.h"
#include "ImageInterfaceImp.h"
#include "OffscreenDrawingSurface.h"
#include "DcDrawingSurface.h"
#include "Path.h"
#include "CompositeSelection.h"

// ****************************************************************************
//
//  Function Name:	RImageView::RImageView( )
//
//  Description:		Constructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImageView::RImageView(const YComponentBoundingRect& rBoundingRect, RComponentDocument* pDocument, RView* pParentView)
: RComponentView(rBoundingRect, pDocument, pParentView )
{	
	// Dont cache images
	m_pRenderCache->Enable( FALSE );
}

// ****************************************************************************
//
//  Function Name:	RImageView::OnXEditComponent( )
//
//  Description:		Process an Edit command on this component
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
void RImageView::OnXEditComponent( EActivationMethod, const RRealPoint& )
{	
}

// ****************************************************************************
//
//  Function Name:	RImageView::Scale()
//
//  Description:		Resize the view about the specified point (bitmaps do
//							not scale, they resize)
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
void RImageView::Scale(const RRealPoint& rCenterOfScaling, const RRealSize& rScaleFactor, BOOLEAN fMaintainAspectRatio)
{
	// Resize
	if( GetParentView( ) )
		{
		YComponentBoundingRect tmp( GetBoundingRect() );
		tmp.UnrotateAndResizeAboutPoint(rCenterOfScaling, rScaleFactor, GetMinimumSize(), GetMaximumSize( ), fMaintainAspectRatio);
		SetBoundingRect( tmp );
		}
	else
		{
		YComponentBoundingRect tmp( GetBoundingRect() );
		tmp.UnrotateAndResizeAboutPoint(rCenterOfScaling, rScaleFactor );
		SetBoundingRect( tmp );
		}

	//	Undefine the outline path
	UndefineOutlinePath( );
}

// ****************************************************************************
//
//  Function Name:	RImageView::CopyBoundingRect( )
//
//  Description:		Copies the bounding rect of another component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageView::CopyBoundingRect( RComponentView* pSource )
	{
	// We dont really want the entire bounding rect; just copy the transform
	CopyTransform( pSource );
	}

// ****************************************************************************
//
//  Function Name:	RImageView::CopyTransform( )
//
//  Description:		Copies the transform of another component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageView::CopyTransform( RComponentView* pSource )
	{
	// To copy the transform, we copy the bounding rect, then reset to our
	// default size
	SetBoundingRect( pSource->GetBoundingRect( ) );
	GetComponentDocument( )->ResetToDefaultSize( pSource->GetComponentDocument( )->GetParentDocument( ) );
	}

// ****************************************************************************
//
//  Function Name:	RImageView::GetInterfaceId( )
//
//  Description:		Return TRUE if the view can supports an Interface type for
//							the given commandId.
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RImageView::GetInterfaceId( YCommandID commandId, YInterfaceId& interfaceId ) const
{
	static	CommandIdToInterfaceId componentViewIterfaceMap[] =
	{
		{ COMMAND_MENU_EFFECTS_TINT_100, kTintSettingsInterfaceId },
		{ COMMAND_MENU_EFFECTS_TINT_90,  kTintSettingsInterfaceId },
		{ COMMAND_MENU_EFFECTS_TINT_80,  kTintSettingsInterfaceId },
		{ COMMAND_MENU_EFFECTS_TINT_70,  kTintSettingsInterfaceId },
		{ COMMAND_MENU_EFFECTS_TINT_60,  kTintSettingsInterfaceId },
		{ COMMAND_MENU_EFFECTS_TINT_50,  kTintSettingsInterfaceId },
		{ COMMAND_MENU_EFFECTS_TINT_40,  kTintSettingsInterfaceId },
		{ COMMAND_MENU_EFFECTS_TINT_30,  kTintSettingsInterfaceId },
		{ COMMAND_MENU_EFFECTS_TINT_20,  kTintSettingsInterfaceId },
		{ COMMAND_MENU_EFFECTS_TINT_10,  kTintSettingsInterfaceId }
	} ;

	for (int i = 0; i < sizeof(componentViewIterfaceMap)/sizeof(CommandIdToInterfaceId); ++i )
	{
		if ( commandId == componentViewIterfaceMap[i].commandId )
		{
			interfaceId = componentViewIterfaceMap[i].interfaceId;
			//	Special case vector images to not allow tinting...
			if ( interfaceId == kTintSettingsInterfaceId )
			{
				RVectorImage*	pImage	= dynamic_cast<RVectorImage*>( GetImageDocument( )->GetImage( ) );
				if ( pImage )
					return FALSE;	//	Vectors can't tint.
			}
			return static_cast<BOOLEAN>(interfaceId != kInvalidInterfaceId);
		}
	}

	//	Nothing found
	return RComponentView::GetInterfaceId( commandId, interfaceId );
}

// ****************************************************************************
//
//  Function Name:	RImageView::GetInterface( )
//
//  Description:		Return The Interface pointer if the given component can support
//							the desired interface.
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RInterface* RImageView::GetInterface( YInterfaceId interfaceId ) const
{
	static	InterfaceToCreateFunction interfaceCreationMap[] =
		{
			{ kImageInterfaceId, RImageInterfaceImp::CreateInterface },
			{ kEditImageInterfaceId, REditImageInterfaceImp::CreateInterface },
			{ kImageTypeInterfaceId, RImageTypeInterfaceImp::CreateInterface },
			{ kObjectPropertiesInterfaceId,	RImagePropertiesImp::CreateInterface }
		} ;

	// Check to see which interfaces we should support
	if ( interfaceId == kTintSettingsInterfaceId || interfaceId == kEditImageInterfaceId )
	{
		if ( !GetImageDocument( )->QueryEditImage( ) ) return NULL;
	}

	
	TpsAssert( interfaceId != kInvalidInterfaceId, "Invalid InterfaceId passed to GetInterface" )
	for (int i = 0; i < sizeof(interfaceCreationMap)/sizeof(InterfaceToCreateFunction); ++i )
		{
		if ( interfaceId == interfaceCreationMap[i].interfaceId )
			return interfaceCreationMap[i].creator( this );
		}

	return RComponentView::GetInterface( interfaceId );
}


// ****************************************************************************
//
//  Function Name:	RImageView::EnableRenderCacheByDefault( )
//
//  Description:		Accessor
//
//  Returns:			TRUE if the render cache is enabled by default for this
//							component
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RImageView::EnableRenderCacheByDefault( ) const
	{
	return FALSE;
	}

#ifdef TPSDEBUG
// ****************************************************************************
//
//  Function Name:	RImageView::Validate( )
//
//  Description:		Validate the object.
//							
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageView::Validate() const
{	
	TpsAssertIsObject(RImageView, this);
}
#endif TPSDEBUG
// ****************************************************************************
//
//  Function Name:	RImageView::HitTest( )
//
//  Description:		Determines if the specified point lies within this component
//							If a clipping path is present we see if the point is within
//							the path.
//
//  Returns:			TRUE:		The point is within this component
//							FALSE:	The point is not within this component
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RImageView::HitTest( const RRealPoint& point ) const
{
	BOOLEAN	fHitTest	= RComponentView::HitTest( point );

	//	If it did not hit the component, just immediately return FALSE
	if ( !fHitTest )
		return FALSE;

	//	If there is a background color, there are no transparencies
	if ( HasBackgroundColor( ) )
		return TRUE;

	//	If the object is already selected, it is a hit.
	RView*	pParentView	= GetParentView( );
	if ( pParentView && pParentView->GetSelection( )->IsSelected( this ) )
		return TRUE;

	// If the object has a path lets look in the path for a hit.
	RBitmapImage*	pBitmap	= dynamic_cast<RBitmapImage*>( GetImageDocument( )->GetImage( ) );
	if(pBitmap)
	{
		RPath* rPath = (RPath*)pBitmap->GetClippingRPath();
		if(rPath)
		{
			fHitTest = PointInPath(point, rPath);

			// If no hit, look for a "filled" frame (empty frames are ignored so we can click through them)
			// WB We only test for frames that are filled with a solid color or gradient.
			if(!fHitTest)
			{
				RPath* rFramePath	= pBitmap->GetFrameRPath();
				if(rFramePath)
				{
					RColor crFrameFillColor = GetComponentDocument( )->GetFrameFillColor();
					if(crFrameFillColor.GetFillMethod() == RColor::kGradient || crFrameFillColor.GetSolidColor())
					{
						fHitTest = PointInPath(point, rFramePath);
					}
				}
			}
		}
	}
	return fHitTest;
}
// ****************************************************************************
//
//  Function Name:	RImageView::PointInPath( )
//
//  Description:		Determines if the specified point lies within this path
//
//  Returns:			TRUE:		The point is within this path
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RImageView::PointInPath(const RRealPoint& point, RPath* rPath ) const
{
	YComponentBoundingRect boundingRect	= GetBoundingRect( );
	BOOLEAN fHitTest = FALSE;
		
	const		YIntDimension	kClickThroughSlopSize	= 4;// centered on mouse point
	const		YIntDimension	kBitmapSize					= 8;//	minimum size (must be greater than slop size)
	const		YIntDimension	kBitsPerPixel				= 8;//	If this is changed, modify the bottom loop
	const		RIntRect			rRender(0,0,kBitmapSize,kBitmapSize);

	//	Render path as a mask into an offscreen at screen DPI.
	static	RBitmapImage	_rMask;
	static	BOOLEAN			_fMaskInitialized	= FALSE;
	if ( !_fMaskInitialized )
	{
		_rMask.Initialize(kBitmapSize, kBitmapSize, kBitsPerPixel);
		_fMaskInitialized = TRUE;
	}
	//	Get the views scale factor
	RRealSize	screenDPI	= GetDPI();
	R2dTransform	transform( GetBoundingRect().GetTransform() );
	transform.Invert( );
	RRealPoint	localPoint	= point*transform;//ConvertPointToLocalCoordinateSystem(point);

	// Modify the mask so we can use it to hold the filled rPath. kBlack makes everything transparent,
	// then open up just the clipping area with the path data in kWhite.
	ROffscreenDrawingSurface rMaskDS;
	rMaskDS.SetImage(&_rMask);
	rMaskDS.SetFillColor(RSolidColor(kBlack));
	rMaskDS.FillRectangle(rRender );

	uBYTE*	pRawData			= (uBYTE*)_rMask.GetRawData( );
	uBYTE*	pBuffer			= (uBYTE*)RBitmapImage::GetImageData( pRawData );
	uBYTE		whiteData		= *pBuffer;

	//	Move transform to beginning of hit rect
	transform.MakeIdentity();
	transform.PreTranslate( kClickThroughSlopSize/2.0, kClickThroughSlopSize/2.0 );
	transform.PreScale( screenDPI.m_dx/(YRealDimension)kSystemDPI, screenDPI.m_dy/(YRealDimension)kSystemDPI);
	transform.PreTranslate( -localPoint.m_x, -localPoint.m_y );

	RRealSize	size = GetReferenceSize( );
	RIntRect NotUsed(0,0,0,0);	// Bounds checker complains about passing a NULL to this param.
	rPath->Render(rMaskDS, size, transform, NotUsed, kFilledAndStroked, kBlack, kWhite, 2);

	rMaskDS.ReleaseImage();
	uLONG		ulBytesPerRow	= RBitmapImage::GetBytesPerRow( kBitmapSize, kBitsPerPixel );

	//	Data is stored as 32 bytes of 8-bit data
	//	If any of the data not WHITE, assume it hit part of the image and return TRUE
	YCounter	i,j;
	fHitTest	= FALSE;		//	Assume we did not hit the object.

	//	Since DIB data is bottom up, advance to the bottom of the buffer
	pBuffer	+= (ulBytesPerRow * (kBitmapSize-1));
	for(i = 0; i < kClickThroughSlopSize; ++i, pBuffer -= ulBytesPerRow )
	{
		uBYTE* pData = pBuffer;
		for(j = 0; j < kClickThroughSlopSize; ++j, ++pData)
		{
			if(*pData != whiteData)
			{
				fHitTest = TRUE;
				break; // gotcha!
			}
		}
	}
	return fHitTest;
}

// ****************************************************************************
//
//  Function Name:	RImageView::CalculateOutlinePath( )
//
//  Description:		Calculate the outline path for this object.
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RImageView::CalculateOutlinePath( )
{
	RImageDocument*	pDoc = GetImageDocument( );

	// get the clipping path from the bitmap image
	RBitmapImage*	pBitmap	= dynamic_cast<RBitmapImage*>( GetImageDocument( )->GetImage( ) );
	if( pBitmap && (GetFrameType( )==kNoFrame) && !HasBackgroundColor( ) )
	{
		TpsAssert( m_pOutlinePath, "CalculateOutlinePath called but path is null" );

		// make sure the outline path is undefined
		m_pOutlinePath->Undefine( );

		RPath *pPath = (RPath*)pBitmap->GetClippingRPath();
		// if we have a path copy it to the outline path
		if( pPath )
		{
			R2dTransform transform;

			// Scale the transform so that we are mapping pixels to device units	
			transform.PreScale((YFloatType)GetBoundingRect( ).Width() / (YFloatType)pBitmap->GetWidthInPixels(), 
										(YFloatType)GetBoundingRect( ).Height() / (YFloatType)pBitmap->GetHeightInPixels() );
			// translate the bitmap origin
			transform.PreTranslate( 0, pPath->GetPathSize().m_dy );
			transform.PreScale(1, -1 );
			m_pOutlinePath->Append( *pPath, transform );
		}
		else	// use base class method
			RComponentView::CalculateOutlinePath( );
	}
	else // use base class method
		RComponentView::CalculateOutlinePath( );
}