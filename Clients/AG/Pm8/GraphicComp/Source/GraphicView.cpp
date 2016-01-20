// ****************************************************************************
//
//  File Name:			GraphicView.cpp
//
//  Project:			Graphic Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RGraphicView class
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
//  $Logfile:: /PM8/GraphicComp/Source/GraphicView.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GraphicIncludes.h"
ASSERTNAME

#include	"GraphicView.h"
#include "FrameworkResourceIDs.h"
#include "GraphicCompResource.h"

#include	"GraphicDocument.h"
#include "EditGraphicAction.h"
#include "SingleSelection.h"
#include "ColorSettings.h"
#include "BackgroundColorSettings.h"
#include "ChangeColorAction.h"
#include "GraphicInterfaceImp.h"
#include "Psd3TimepieceGraphic.h"
#include "TimepiecePlacementInterfaceImp.h"
#include "Psd3GraphicInterfaceImp.h"
#include "Path.h"
#include "Menu.h"
#include "ComponentTypes.h"
#include "Psd3BorderGraphic.h"
#include "BorderView.h"
#include "BitmapImage.h"
#include "OffscreenDrawingSurface.h"
#include "DcDrawingSurface.h"

// ****************************************************************************
//
//  Function Name:	RGraphicView::RGraphicView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGraphicView::RGraphicView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView )
	: RComponentView( boundingRect, pDocument, pParentView )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RGraphicView::OnXEditComponent( )
//
//  Description:		Process an EditCommand on the component.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGraphicView::OnXEditComponent( EActivationMethod, const RRealPoint& )
	{
	try
		{
		RGraphicDocument*		pDoc		= GetGraphicDocument();
		REditGraphicAction*	pAction	= new REditGraphicAction( pDoc );
		pDoc->SendAction( pAction );
		}
	catch( ... )
		{
		///xxx	TODO: Report that the Graphic could not be edited...
		NULL;
		}
	}

// ****************************************************************************
//
//  Function Name:	RGraphicView::GetGraphicDocument( )
//
//  Description:		Accessor
//							
//  Returns:			This views document, cast to a graphic document
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGraphicDocument* RGraphicView::GetGraphicDocument( ) const
	{
	TpsAssertIsObject( RGraphicDocument, GetRDocument( ) );
	return static_cast<RGraphicDocument*>( GetRDocument( ) );
	}


// ****************************************************************************
//
//  Function Name:	RGraphicView::HitTest( )
//
//  Description:		Determine if the object was hit (in a valid portion of the document)
//							
//  Returns:			This views document, cast to a graphic document
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGraphicView::HitTest(const RRealPoint& point ) const
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

	//	Get the graphic to image
	RGraphic*	pGraphic	= GetGraphicDocument( )->GetGraphic( );
	//	If no graphic, it must be a placeholder, always hit it...
	if ( pGraphic )
		{
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
		RRealPoint		localPoint	= point*transform;//ConvertPointToLocalCoordinateSystem(point);

		ROffscreenDrawingSurface rMaskDS;
		rMaskDS.SetImage(&_rMask);
			//	Clear drawingsurface to white...
		rMaskDS.SetFillColor( RSolidColor(kWhite) );
		rMaskDS.FillRectangle( rRender );

		uBYTE*	pRawData			= (uBYTE*)_rMask.GetRawData( );
		uBYTE*	pBuffer			= (uBYTE*)RBitmapImage::GetImageData( pRawData );
		uBYTE		whiteData		= *pBuffer;

		//	Move transform to beginning of hit rect
		transform.MakeIdentity();
		transform.PreTranslate( kClickThroughSlopSize/2.0, kClickThroughSlopSize/2.0 );
		transform.PreScale( screenDPI.m_dx/(YRealDimension)kSystemDPI, screenDPI.m_dy/(YRealDimension)kSystemDPI);
		transform.PreTranslate( -localPoint.m_x, -localPoint.m_y );

		RRealSize	size = GetReferenceSize( );
		pGraphic->Render( rMaskDS, size, transform, rRender, RSolidColor( kBlack ), TRUE );

		//	Check if the entire mask bitmap is white
		rMaskDS.ReleaseImage();

		uLONG		ulBytesPerRow	= RBitmapImage::GetBytesPerRow( kBitmapSize, kBitsPerPixel );
		//	data is stored as 32 bytes of 8-bit data
		//		If any of the data not WHITE, assume it hit part of the
		//		image and return TRUE
		YCounter	i,j;
		fHitTest	= FALSE;		//	Assume we did not hit the object.
		//	since DIB data is bottom up, advance to the bottom of the buffer
		pBuffer	+= (ulBytesPerRow * (kBitmapSize-1));
		for ( i = 0; i < kClickThroughSlopSize; ++i, pBuffer -= ulBytesPerRow )
			{
			uBYTE* pData = pBuffer;
			for ( j = 0; j < kClickThroughSlopSize; ++j, ++pData )
				if ( *pData != whiteData )
					fHitTest = TRUE;
			}
		}
	return fHitTest;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetColor( )
//
//  Description:		Get the color of the view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGraphicView::GetColor( RColor& color ) const
	{
	RGraphicDocument*		pDocument	= GetGraphicDocument( );
	RGraphic*				pGraphic		= pDocument->GetGraphic( );
	if( pGraphic )
		color = pGraphic->GetMonochromeColor( );
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RGraphicView::OnXDisplayChange( )
//
//  Description:		Called when the display resolution changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGraphicView::OnXDisplayChange( )
	{
	RGraphicDocument*		pDocument	= GetGraphicDocument( );
	RGraphic*				pGraphic		= pDocument->GetGraphic( );
	if (pGraphic)
		pGraphic->OnXDisplayChange( );
	}

// ****************************************************************************
//
//  Function Name:	RGraphicView::ApplyColor( )
//
//  Description:		Apply a color to the view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGraphicView::ApplyColor( const RColor& color )
	{
	RGraphicDocument*		pDocument	= GetGraphicDocument( );
	RGraphic*				pGraphic		= pDocument->GetGraphic( );
	RChangeColorAction*	pAction		= new RChangeColorAction( pDocument, pGraphic, color );
	pDocument->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RGraphicView::GetInterfaceId( )
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
BOOLEAN RGraphicView::GetInterfaceId( YCommandID commandId, YInterfaceId& interfaceId ) const
	{
	static	CommandIdToInterfaceId componentViewIterfaceMap[] =
		{
			{ COMMAND_MENU_EFFECTS_COLOR_DLG,		kColorSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_COLOR_OBJECT,	kColorSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_COLOR_BLACK,		kColorSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_COLOR_WHITE,		kColorSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_COLOR_YELLOW,	kColorSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_COLOR_MAGENTA,	kColorSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_COLOR_RED,		kColorSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_COLOR_CYAN,		kColorSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_COLOR_GREEN,		kColorSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_COLOR_BLUE,		kColorSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_COLOR_LTGRAY,	kColorSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_COLOR_GRAY,		kColorSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_COLOR_DKGRAY,	kColorSettingsInterfaceId }
		} ;

	for (int i = 0; i < sizeof(componentViewIterfaceMap)/sizeof(CommandIdToInterfaceId); ++i )
		{
		if ( commandId == componentViewIterfaceMap[i].commandId )
			{
			interfaceId = componentViewIterfaceMap[i].interfaceId;
			//		Make sure the graphic is Monochrome IF interface is ColorSettings
			if ( interfaceId == kColorSettingsInterfaceId )
				{
				RGraphic*	pGraphic	= GetGraphicDocument( )->GetGraphic( );
				if ( !pGraphic || !pGraphic->IsMonochrome( ) )
					return FALSE;
				}
			return static_cast<BOOLEAN>(interfaceId != kInvalidInterfaceId);
			}
		}

	//	Nothing found
	return RComponentView::GetInterfaceId( commandId, interfaceId );
	}

// ****************************************************************************
//
//  Function Name:	RGraphicView::GetInterface( )
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
RInterface* RGraphicView::GetInterface( YInterfaceId interfaceId ) const
	{
	static	InterfaceToCreateFunction interfaceCreationMap[] =
		{
			{ kColorSettingsInterfaceId, RColorSettingsInterface::CreateInterface },
			{ kGraphicInterfaceId, RGraphicInterfaceImp::CreateInterface },
			{ kGraphicColorInterfaceId, RGraphicColorInterfaceImp::CreateInterface },
			{ kDateTimeInterfaceId, RGraphicDateTimeInterfaceImp::CreateInterface },
			{ kTimepiecePlacementInterfaceId, RTimepiecePlacementInterfaceImp::CreateInterface },
			{ kPsd3GraphicInterfaceId, RPsd3GraphicInterfaceImp::CreateInterface },
			{ kObjectPropertiesInterfaceId,	RGraphicPropertiesImp::CreateInterface }
		} ;
	
	if ( interfaceId == kColorSettingsInterfaceId )
		{
		// Make sure the graphic is Monochrome IF interface is ColorSettings
		RGraphic*	pGraphic	= GetGraphicDocument( )->GetGraphic( );
		if ( !pGraphic || !pGraphic->IsMonochrome( ) )
			return NULL;
		}	
	else 
	if ( interfaceId == kDateTimeInterfaceId )
		{
		// Make sure that the graphic is a timepiece if we are asked for a kDateTimeInterfaceId
		RPsd3TimepieceGraphic* pTimepiece = dynamic_cast<RPsd3TimepieceGraphic*>( GetGraphicDocument( )->GetGraphic( ) );
		if (!pTimepiece || pTimepiece->IsDigital()) return NULL;
		}
	else
	if ( interfaceId == kTimepiecePlacementInterfaceId )
		{
		// Make sure that the graphic is a timepiece if we are asked for a kTimepiecePlacementInterfaceId
		RPsd3TimepieceGraphic* pTimepiece = dynamic_cast<RPsd3TimepieceGraphic*>( GetGraphicDocument( )->GetGraphic( ) );
		if (!pTimepiece) return NULL;
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
//  Function Name:	RGraphicView::CalculateOutlinePath( )
//
//  Description:		Calculate the outline path for this object.
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RGraphicView::CalculateOutlinePath( )
	{
	RGraphicDocument*				pDoc				= GetGraphicDocument( );
	RGraphic*						pGraphic			= pDoc->GetGraphic();

	if( pGraphic && (GetFrameType( )==kNoFrame) && !HasBackgroundColor( ) )
		{
		TpsAssert( m_pOutlinePath, "CalculateOutlinePath called but path is null" );

		//	If this is a border, set its thickness
		YComponentType	hType	= pDoc->GetComponentType( );
		if (hType == kBorderComponent || hType == kMiniBorderComponent)
		{
			RBorderView* pBorderView = static_cast<RBorderView*>(this);
			YComponentBoundingRect rBorderExteriorBoundingRect = pBorderView->GetBoundingRect();
			YComponentBoundingRect rBorderInteriorBoundingRect = pBorderView->GetInteriorBoundingRect();
			YRealDimension nBorderThickness = (rBorderExteriorBoundingRect.Width() - rBorderInteriorBoundingRect.Width()) / 2.0;

			// Get a pointer to the border graphic
			RPsd3BorderGraphic* pBorderGraphic = dynamic_cast<RPsd3BorderGraphic*>(pGraphic);

			// Set the border thickness
			pBorderGraphic->SetBorderThickness(nBorderThickness);
		}

		//	First, make sure the outline is undefined
		m_pOutlinePath->Undefine( );
		if ( !pGraphic->GetOutlinePath(*m_pOutlinePath, GetReferenceSize( ), GetBoundingRect().GetTransform( )) )
			RComponentView::CalculateOutlinePath( );
		}
	else
		RComponentView::CalculateOutlinePath( );
	}

// ****************************************************************************
//
//  Function Name:	RBackdropView::RBackdropView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBackdropView::RBackdropView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView )
	: RGraphicView( boundingRect, pDocument, pParentView )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RBackdropView::Scale( )
//
//  Description:		Resize the view about the specified point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBackdropView::Scale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio )
	{
	// Invalidate the area we used to occupy
	Invalidate( );

	// Resize
	if( GetParentView( ) )
		{
		YComponentBoundingRect tmp( GetBoundingRect() );
		tmp.ResizeAboutPoint( centerOfScaling, ApplyResizeConstraint( scaleFactor ), GetMinimumSize( ), GetMaximumSize( ), fMaintainAspectRatio );
		SetBoundingRect( tmp );
		}
	else
		{
		YComponentBoundingRect tmp( GetBoundingRect() );
		tmp.ResizeAboutPoint( centerOfScaling, ApplyResizeConstraint( scaleFactor ) );
		SetBoundingRect( tmp );
		}

	//	Undefine the cached outlinepath
	UndefineOutlinePath( );

	// Invalidate the area we now occupy
	Invalidate( );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RGraphicView::Validate( )
//
//  Description:		Validate the object.
//							
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGraphicView::Validate( ) const
	{
	RComponentView::Validate( );
	TpsAssertIsObject( RGraphicView, this );
	}

#endif	//	TPSDEBUG

