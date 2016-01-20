// ****************************************************************************
//
//  File Name:			update.c
//
//  Project:			Renaissance Graphic Headline Renderer
//
//  Author:				R. Hood
//
//  Description:		Shell application screen and printer drawing routines
//
//  Portability:		Macintosh specific
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
//  $Logfile:: /Development/HeadlineComp/MacShell/Source/UPDATE.C    $
//	 $Author:: Richh                                                  $
//	 $Date:: 5/21/97 12:27 PM                                         $
//  $Revision:: 16                                                   $
//
// ****************************************************************************


#include		<Windows.h>
#include		<Printing.h>
#include	 	"HeadlinerIncludes.h"

ASSERTNAME

#include	 	"GpDrawingSurface.h"
#include	 	"PrinterDrawingSurface.h"
#include	 	"PictureDrawingSurface.h"
#include	 	"PostScriptDrawingSurface.h"
#include	 	"Psd3Graphic.h"
#include		"Psd3BorderGraphic.h"
#include 	"VectorRect.h"
#include		"GradientFill.h"
#include		"HeadlineGraphic.h"
#include		"tpsbit.h"
#include		"menu.h"
#include		"event.h"
#include		"update.h"

/**  local defines  **/
#define  kPostScriptPrinterVersion			3

/**  local prototypes  **/
static	void			  _ApplyUserScroll( R2dTransform& transform );
static	void			  _ApplyUserScale( R2dTransform& transform, RIntRect* rect, RIntSize& dpi );
static	void			  _ApplyPrinterScale( RIntRect* rect, RPrinterDrawingSurface& drawingSurface, BOOLEAN fUseMaxRes );
static  	RIntRect  	  _ApplyRotationCompensation ( R2dTransform& transform, const RIntRect& graphicRect, YAngle rotationAngle );
static	BOOLEAN		  _fIsPostScript ( THPrint hPrintInfo );
static	void		 	  _DrawPolygon( void );
         void           DrawGradientToScreen ( WindowPtr pPort, Rect *rect );
         void           DrawFeedbackToScreen ( WindowPtr pPort );

/**  globals  **/
extern  WindowPtr			 	gMainWindow;
extern  TEHandle				gMainTE;
extern  THPrint				hPrintInfo;
extern  RHeadlineGraphic*	gpHeadline;
extern  RGraphic*				gpGraphic;
extern  RPath*					gpWarpPath;
extern  RGradientFill*		gpGradientBackdrop;
extern  Handle					ghbitGraphic;
extern  short				 	gScaleMenuItem;
extern  short				 	gFlipMenuItem;
extern  float				 	gRotateDegrees;
extern  BOOLEAN  			 	gfUseOffScreen;
extern  EFlipDirection		gFlipDirection;
extern  RIntSize				gScroll;
extern  BOOLEAN 				gfFeedback;

short								gGraphicType;

/**  locals  **/
static  Boolean				_fShowWarpEnvelop = false;
static  Boolean				_fUsePostScript = false;


/* ----------------------------------------------------------------------------
	PaintMainWindow 
	Process an update event to the main window.
	Returns nothing.													 
	---------------------------------------------------------------------------- */
void  PaintMainWindow (void)
{
GrafPtr			savePort;
Rect				rect;
RgnHandle		hClipRgn = NewRgn();

	if ( gMainWindow == nil )  											// no window 
		return;
	GetPort(&savePort);
	SetPort(gMainWindow);

	if ( gfUseOffScreen )													// create off screen bitmap rep
		if ( gpGraphic != nil || gpHeadline != nil )
			if ( ghbitGraphic == nil )
				DrawGraphicToOffScreen();

	SetPort(gMainWindow);
	if ( hClipRgn != nil )													// clip out scroll bars
	{
		rect = gMainWindow->portRect;
		rect.right	-= 15;
		rect.bottom	-= 15;
		GetClip(hClipRgn);
		ClipRect(&rect);
	}

	// Update gradients
/*	if (gpHeadline != nil)*/
/*	{*/
/*		RGpDrawingSurface		drawingSurface;*/
/**/
/*		gpHeadline->Redefine();*/
/*		gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );*/
/*	}*/
	
	if ( gpGraphic == nil && gpHeadline == nil ) 					// no workspace
	{
		if ( gpGradientBackdrop == nil ) 
			FillRect( &(gMainWindow->portRect), &qd.white );
		else
			DrawGradientToScreen( gMainWindow, &rect );
	}
	else if ( gfFeedback ) 	 
	{
		if ( gpHeadline != nil ) 	 
			DrawFeedbackToScreen( gMainWindow );
		else
			DrawGraphicToScreen( gMainWindow, &rect );
	}
	else
	{
		long lTicks = TickCount();
		DrawGraphicToScreen( gMainWindow, &rect );
		lTicks = TickCount() - lTicks;
	}

	if ( hClipRgn != nil )													// reset clip region
	{
		SetPort(gMainWindow);
		SetClip(hClipRgn);
		DisposeRgn(hClipRgn);
	}

	rect = gMainWindow->portRect;
	rect.left = rect.right - 15;
	FillRect( &rect, &qd.white );
	rect = gMainWindow->portRect;
	rect.top = rect.bottom - 15;
	FillRect( &rect, &qd.white );
	DrawGrowIcon(gMainWindow);

	SetCursor(&qd.arrow);
	SetPort(savePort);
}


/* ----------------------------------------------------------------------------
	PrintMainWindow 
	Process a print request. Create printer drawing surface and render graphic.
	Returns nothing.													 
	---------------------------------------------------------------------------- */
void  PrintMainWindow (void)
{
GrafPtr				  		savePort;
RPrinterDrawingSurface	*pDrawingSurface = nil;
R2dTransform				transform;
RIntRect						graphicRect( 0, 0, 0, 0 );
RIntRect						graphicOutRect;
RIntSize 					graphicSize;
RIntPoint					ptCenter;
Rect							rect;

/**  no window or printer  **/
	if ( gMainWindow == nil || hPrintInfo == nil )  
		return;
	GetPort( &savePort );
		 
/**  open printer  **/
	PrOpen( );
	if ( PrError() != noErr )
		return;

/**  print dialog  **/
	PrValidate( hPrintInfo );
	if ( !PrJobDialog( hPrintInfo ) )
		goto error;
	if ( gpGraphic == nil && gpHeadline == nil )
		goto error;

/**  instanciate drawing surface  **/
	if ( _fIsPostScript( hPrintInfo ) && _fUsePostScript )
		pDrawingSurface = new RPostScriptDrawingSurface( hPrintInfo );
	else
		pDrawingSurface = new RPrinterDrawingSurface( hPrintInfo );

/**  setup graphic and drawing surface for output  **/
	if ( gpGraphic == nil )
		graphicSize = gpHeadline->GetGraphicSize();
	else
	{
		switch (gGraphicType)
		{
			case RGraphicInterface::kSquare :
			case RGraphicInterface::kRow :
			case RGraphicInterface::kColumn :
			case RGraphicInterface::kSeal :
			case RGraphicInterface::kBackdrop :
				graphicSize = ((RSingleGraphic*)gpGraphic)->GetGraphicSize();
				ptCenter.m_x = graphicSize.m_dx / 2;
				ptCenter.m_y = graphicSize.m_dy / 2;
				graphicRect = RIntRect( graphicSize );
				break;

			case RGraphicInterface::kBorder :
				((RPsd3BorderGraphic*)gpGraphic)->SetBorderThickness( 1440 );
				rect 					= gMainWindow->portRect;
				graphicSize.m_dx	= rect.right - rect.left - 20;
				graphicSize.m_dy	= rect.bottom - rect.top - 20;

				ptCenter.m_x		= graphicSize.m_dx / 2;
				ptCenter.m_y		= graphicSize.m_dy / 2;
				graphicRect			= RIntRect( graphicSize );
				break;

			case RGraphicInterface::kLine :
				rect 					= gMainWindow->portRect;
				graphicSize.m_dx	= rect.right - rect.left;
				graphicSize.m_dy	= 72;
				ptCenter.m_x		= graphicSize.m_dx / 2;
				ptCenter.m_y		= graphicSize.m_dy / 2;
				graphicRect			= RIntRect( graphicSize );
				break;

			case RGraphicInterface::kTimepiece :
				graphicSize.m_dx	= 300;
				graphicSize.m_dy	= 300;
				ptCenter.m_x		= graphicSize.m_dx / 2;
				ptCenter.m_y		= graphicSize.m_dy / 2;
				graphicRect			= RIntRect( graphicSize );
				break;

			case RGraphicInterface::kHorzBannerBackdrop :
				rect 					= gMainWindow->portRect;
				graphicSize.m_dx	= rect.right - rect.left - 20;
				graphicSize.m_dy	= rect.bottom - rect.top - 20;

				ptCenter.m_x		= graphicSize.m_dx / 2;
				ptCenter.m_y		= graphicSize.m_dy / 2;
				graphicRect			= RIntRect( graphicSize );
				break;
				
			case RGraphicInterface::kVertBannerBackdrop :
				rect 					= gMainWindow->portRect;
				graphicSize.m_dx	= rect.right - rect.left - 20;
				graphicSize.m_dy	= rect.bottom - rect.top - 20;

				ptCenter.m_x		= graphicSize.m_dx / 2;
				ptCenter.m_y		= graphicSize.m_dy / 2;
				graphicRect			= RIntRect( graphicSize );
				break;
				
			case RGraphicInterface::kTiledBannerBackdrop :
				TpsAssertAlways( "Tiled Backdrop Graphic's are not implemented yet!" );
				goto error;
				break;

			default:
				TpsAssertAlways( "Library type not supported for printing yet!" );
				goto error;
				break;
		}	
	}
	
	transform.PreRotateAboutPoint( ptCenter.m_x, ptCenter.m_y, DegreesToRadians( gRotateDegrees ) );
	_ApplyUserScale( transform, &graphicRect, pDrawingSurface->GetMaxDPI( TRUE ) );
	_ApplyPrinterScale( &graphicRect, *pDrawingSurface, true );
	if ( gFlipDirection == kFlipHorizontal )
	{
		transform.PreTranslate( graphicRect.m_Right, 0 );
		transform.PreScale( -1, 1 );
	}
	else if ( gFlipDirection == kFlipVertical )
	{
		transform.PreTranslate( 0, graphicRect.m_Bottom );
		transform.PreScale( 1, -1 );
	}
	else if ( gFlipDirection == kFlipHorzAndVert )
	{
		transform.PreTranslate( graphicRect.m_Right, graphicRect.m_Bottom );
		transform.PreScale( -1, -1 );
	}
	graphicOutRect = _ApplyRotationCompensation( transform, graphicRect, gRotateDegrees );
	if ( gpHeadline != nil )
		gpHeadline->Construct( RRealSize( graphicRect.m_Right, graphicRect.m_Bottom ), *pDrawingSurface );

/**  initalize drawing surface and print port  **/
	SetCursor( *(GetCursor( watchCursor )) );
	try
	{
		if ( !pDrawingSurface->Initialize( (GrafPtr)gMainWindow, graphicOutRect, true ) )
			goto error;

/**  render graphic  **/
		if ( pDrawingSurface->BeginPage( ) )
		{
			if ( gpGraphic == nil )
				gpHeadline->Render( *pDrawingSurface, RRealSize( graphicRect.m_Right, graphicRect.m_Bottom ), transform, RIntRect( -16384, -16384, 16384, 16384 ) );
			else
				gpGraphic->Render( *pDrawingSurface, RRealSize( graphicRect.m_Right, graphicRect.m_Bottom ), transform, RIntRect( -16384, -16384, 16384, 16384 ) );
			pDrawingSurface->EndPage( );
		}
	}
	catch ( YException except )
	{
		switch ( except ) 
		{
			case kMemory:
				StopAlert( rLowMemoryALRT, nil );							// out of memeory
				break;
			default:
				AlertOnError( stopIcon, 17000, false );					// generic application file error
				break;
		}
	}
	catch ( ... )
	{
		AlertOnError( stopIcon, 19001, false );							// should not happen
	}

/**  close and clean-up  **/
error:																				// yuk, gotos
	if ( pDrawingSurface != nil )
		delete pDrawingSurface;
	PrClose( );
	SetCursor( &qd.arrow );
	SetPort( savePort );
	if ( gpHeadline != nil )
	{
		RGpDrawingSurface	drawingSurface;
		gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
	}
}


/* ----------------------------------------------------------------------------
	DrawGraphicToScreen 
	Create screen drawing surface and render graphic.
	Returns nothing.													 
	---------------------------------------------------------------------------- */
void  DrawGraphicToScreen ( WindowPtr pPort, Rect *pClipRect )
{
RGpDrawingSurface		drawingSurface;
R2dTransform			transform;
RIntRect					graphicRect;
RIntRect					graphicOutRect;
RIntSize 				graphicSize;
RIntPoint				ptCenter;
Rect						rect;

/**  no graphic  **/
	if ( gpGraphic == nil && gpHeadline == nil )  
	{
		rect = gMainWindow->portRect;
		if ( gpGradientBackdrop != nil )  	
			if ( drawingSurface.Initialize( (GrafPtr)pPort ) )
			{
				RIntRect			rcRender( rect.left, rect.top, rect.right, rect.bottom );
				const RColor&	oldColor				= drawingSurface.GetFillColor();
				YPercentage		pctOldPrecision	= drawingSurface.GetGradientPrecision();
				drawingSurface.SetFillColor( *gpGradientBackdrop );
				drawingSurface.FillRectangle( rcRender, transform );
				drawingSurface.SetFillColor( oldColor );
			}
		return;
	}

/**  use offscreen if available  **/
	if ( ghbitGraphic != nil )
	{
		tbitGetRect( ghbitGraphic, &rect );
		if ( SectRect( &rect, pClipRect, &rect ) )  
			if ( tbitBlitToScreen(ghbitGraphic, &rect, &rect, TBIT_SrcCopy, false) )
				return;
	}
		 
/**  setup graphic for output  **/
	if ( gpGraphic == nil )
		graphicSize = gpHeadline->GetGraphicSize();
	else
	{
		switch (gGraphicType)
		{
			case RGraphicInterface::kSquare :
			case RGraphicInterface::kRow :
			case RGraphicInterface::kColumn :
			case RGraphicInterface::kSeal :
			case RGraphicInterface::kBackdrop :
				graphicSize = ((RSingleGraphic*)gpGraphic)->GetGraphicSize();

				ptCenter.m_x		= graphicSize.m_dx / 2;
				ptCenter.m_y		= graphicSize.m_dy / 2;
				graphicRect			= RIntRect( graphicSize );
				break;

			case RGraphicInterface::kBorder :
				((RPsd3BorderGraphic*)gpGraphic)->SetBorderThickness( 1440 );
				rect 					= gMainWindow->portRect;
				graphicSize.m_dx	= rect.right - rect.left - 20;
				graphicSize.m_dy	= rect.bottom - rect.top - 20;

				ptCenter.m_x		= graphicSize.m_dx / 2;
				ptCenter.m_y		= graphicSize.m_dy / 2;
				graphicRect			= RIntRect( graphicSize );
				break;

			case RGraphicInterface::kLine :
				rect 					= gMainWindow->portRect;
				graphicSize.m_dx	= rect.right - rect.left;
				graphicSize.m_dy	= 72;
				ptCenter.m_x		= graphicSize.m_dx / 2;
				ptCenter.m_y		= graphicSize.m_dy / 2;
				graphicRect			= RIntRect( graphicSize );
				break;

			case RGraphicInterface::kTimepiece :
				graphicSize.m_dx	= 300;
				graphicSize.m_dy	= 300;
				ptCenter.m_x		= graphicSize.m_dx / 2;
				ptCenter.m_y		= graphicSize.m_dy / 2;
				graphicRect			= RIntRect( graphicSize );
				break;

			case RGraphicInterface::kHorzBannerBackdrop :
				rect 					= gMainWindow->portRect;
				graphicSize.m_dx	= rect.right - rect.left - 20;
				graphicSize.m_dy	= rect.bottom - rect.top - 20;

				ptCenter.m_x		= graphicSize.m_dx / 2;
				ptCenter.m_y		= graphicSize.m_dy / 2;
				graphicRect			= RIntRect( graphicSize );
				break;
				
			case RGraphicInterface::kVertBannerBackdrop :
				rect 					= gMainWindow->portRect;
				graphicSize.m_dx	= rect.right - rect.left - 20;
				graphicSize.m_dy	= rect.bottom - rect.top - 20;

				ptCenter.m_x		= graphicSize.m_dx / 2;
				ptCenter.m_y		= graphicSize.m_dy / 2;
				graphicRect			= RIntRect( graphicSize );
				break;
				
			case RGraphicInterface::kTiledBannerBackdrop :
				TpsAssertAlways( "Tiled Backdrop Graphic's are not implemented yet!" );
				goto exit;
				break;
				
			default:
				TpsAssert( FALSE, "Library type unknown!" );
				goto exit;
				break;
		}
	}

	transform.PreRotateAboutPoint( ptCenter.m_x, ptCenter.m_y, DegreesToRadians( gRotateDegrees ) );
	_ApplyUserScale( transform, &graphicRect, drawingSurface.GetDPI() );

	if ( gFlipDirection == kFlipHorizontal )
	{
		transform.PreTranslate( graphicRect.m_Right, 0 );
		transform.PreScale( -1, 1 );
	}
	else if ( gFlipDirection == kFlipVertical )
	{
		transform.PreTranslate( 0, graphicRect.m_Bottom );
		transform.PreScale( 1, -1 );
	}
	else if ( gFlipDirection == kFlipHorzAndVert )
	{
		transform.PreTranslate( graphicRect.m_Right, graphicRect.m_Bottom );
		transform.PreScale( -1, -1 );
	}
	graphicOutRect = _ApplyRotationCompensation( transform, graphicRect, gRotateDegrees );
	_ApplyUserScroll( transform );

/**  render graphic  **/
	SetCursor( *(GetCursor( watchCursor )) );
	try
	{
		if ( drawingSurface.Initialize( (GrafPtr)pPort ) )
		{
			if ( gpGraphic == nil )  
				gpHeadline->Render( drawingSurface, RRealSize( graphicRect.m_Right, graphicRect.m_Bottom ), transform, RIntRect( -16384, -16384, 16384, 16384 ) );
			else
				gpGraphic->Render( drawingSurface, RRealSize( graphicRect.m_Right, graphicRect.m_Bottom ), transform, RIntRect( -16384, -16384, 16384, 16384 ) );
		}
	}
	catch ( YException except )
	{
		switch ( except ) 
		{
			case kMemory:
				StopAlert( rLowMemoryALRT, nil );							// out of memeory
				break;
			default:
				AlertOnError( stopIcon, 17000, false );					// generic application file error
				break;
		}
	}
	catch ( ... )
	{
		AlertOnError( stopIcon, 19001, false );							// should not happen
	}

exit:
	SetCursor( &qd.arrow );
}


/* ----------------------------------------------------------------------------
	DrawFeedbackToScreen 
	Create screen drawing surface and render graphic.
	Returns nothing.													 
	---------------------------------------------------------------------------- */
void  DrawFeedbackToScreen ( WindowPtr pPort )
{
RGpDrawingSurface		drawingSurface;
R2dTransform			transform;
RIntRect					graphicRect;
RIntRect					graphicOutRect;
RIntSize 				graphicSize;
RIntPoint				ptCenter;

/**  no graphic  **/
	if ( gpHeadline == nil )  
		return;

/**  setup graphic for output  **/
	graphicSize = gpHeadline->GetGraphicSize();
	ptCenter.m_x = graphicSize.m_dx / 2;
	ptCenter.m_y = graphicSize.m_dy / 2;
	graphicRect = RIntRect( graphicSize );
	transform.PreRotateAboutPoint( ptCenter.m_x, ptCenter.m_y, DegreesToRadians( gRotateDegrees ) );
	_ApplyUserScale( transform, &graphicRect, drawingSurface.GetDPI() );
	if ( gFlipDirection == kFlipHorizontal )
	{
		transform.PreTranslate( graphicRect.m_Right, 0 );
		transform.PreScale( -1, 1 );
	}
	else if ( gFlipDirection == kFlipVertical )
	{
		transform.PreTranslate( 0, graphicRect.m_Bottom );
		transform.PreScale( 1, -1 );
	}
	else if ( gFlipDirection == kFlipHorzAndVert )
	{
		transform.PreTranslate( graphicRect.m_Right, graphicRect.m_Bottom );
		transform.PreScale( -1, -1 );
	}
	graphicOutRect = _ApplyRotationCompensation( transform, graphicRect, gRotateDegrees );
	_ApplyUserScroll( transform );

/**  render graphic  **/
	SetCursor( *(GetCursor( watchCursor )) );
	try
	{
		if ( drawingSurface.Initialize( (GrafPtr)pPort ) )
		{
			gpHeadline->RenderFeedback( drawingSurface, RRealSize( graphicRect.m_Right, graphicRect.m_Bottom ), transform );
		}
	}
	catch ( YException except )
	{
		switch ( except ) 
		{
			case kMemory:
				StopAlert( rLowMemoryALRT, nil );							// out of memeory
				break;
			default:
				AlertOnError( stopIcon, 17000, false );					// generic application file error
				break;
		}
	}
	catch ( ... )
	{
		AlertOnError( stopIcon, 19001, false );							// should not happen
	}
	SetCursor( &qd.arrow );
}


/* ----------------------------------------------------------------------------
	DrawGraphicToOffScreen 
	Create off screen drawing surface and render graphic.
	Returns nothing.													 
	---------------------------------------------------------------------------- */
void  DrawGraphicToOffScreen ( void )
{
GrafPtr					savePort;
GrafPtr					pPort;
RGpDrawingSurface		drawingSurface;
R2dTransform			transform;
RIntRect					graphicRect;
RIntRect					graphicOutRect;
RIntSize 				graphicSize;
RIntPoint				ptCenter;
Rect						rect;

/**  no graphic  **/
	if ( gpGraphic == nil && gpHeadline == nil )  
		return;
	if ( ghbitGraphic != nil )  
		return;
	 GetPort(&savePort);
		 
/**  setup graphic for output  **/
	if ( gpGraphic == nil )
		graphicSize = gpHeadline->GetGraphicSize();
	else
		graphicSize = ((RSingleGraphic*)gpGraphic)->GetGraphicSize();
	ptCenter.m_x = graphicSize.m_dx / 2;
	ptCenter.m_y = graphicSize.m_dy / 2;
	graphicRect = RIntRect( graphicSize );
	transform.PreRotateAboutPoint( ptCenter.m_x, ptCenter.m_y, DegreesToRadians( gRotateDegrees ) );
	_ApplyUserScale( transform, &graphicRect, drawingSurface.GetDPI() );
	if ( gFlipDirection == kFlipHorizontal )
	{
		transform.PreTranslate( graphicRect.m_Right, 0 );
		transform.PreScale( -1, 1 );
	}
	else if ( gFlipDirection == kFlipVertical )
	{
		transform.PreTranslate( 0, graphicRect.m_Bottom );
		transform.PreScale( 1, -1 );
	}
	else if ( gFlipDirection == kFlipHorzAndVert )
	{
		transform.PreTranslate( graphicRect.m_Right, graphicRect.m_Bottom );
		transform.PreScale( -1, -1 );
	}
	graphicOutRect = _ApplyRotationCompensation( transform, graphicRect, gRotateDegrees );

/**  graphic too big  **/
	if ( ( graphicOutRect.Width() * graphicOutRect.Height() ) > ( MaxBlock() - 10240 ) )  
		return;

/**  create offscreen  **/
	SetRect( &rect, graphicOutRect.m_Left, graphicOutRect.m_Top, graphicOutRect.m_Right, graphicOutRect.m_Bottom);
	if ( (ghbitGraphic = tbitAlloc( 8, &rect, (TBIT_ZeroInit | TBIT_LongAlignRows | TBIT_DefaultColorTable | TBIT_16ByteAlignRows) )) == nil )
		return;

/**  render graphic  **/
	SetCursor( *(GetCursor( watchCursor )) );
	try
	{
		if ( tbitLock( ghbitGraphic ) )
		{
			GetPort( &pPort );
			FillRect( &rect, &qd.white );										// background color
			if ( drawingSurface.Initialize( pPort ) )
			{
				if ( gpGraphic == nil )  
					gpHeadline->Render( drawingSurface, RRealSize( graphicRect.m_Right, graphicRect.m_Bottom ), transform, RIntRect( -16384, -16384, 16384, 16384 ) );
				else
					gpGraphic->Render( drawingSurface, RRealSize( graphicRect.m_Right, graphicRect.m_Bottom ), transform, RIntRect( -16384, -16384, 16384, 16384 ) );
			}
			tbitUnlock( ghbitGraphic );
		}
		SetPort(savePort);
	}
	catch ( YException except )
	{
		switch ( except ) 
		{
			case kMemory:
				StopAlert( rLowMemoryALRT, nil );							// out of memeory
				break;
			default:
				AlertOnError( stopIcon, 17000, false );					// generic application file error
				break;
		}
	}
	catch ( ... )
	{
		AlertOnError( stopIcon, 19001, false );							// should not happen
	}
	SetCursor( &qd.arrow );
}


/* ----------------------------------------------------------------------------
	DrawGradientToScreen 
	Create screen drawing surface and render gradient.
	Returns nothing.													 
	---------------------------------------------------------------------------- */
void  DrawGradientToScreen ( WindowPtr pPort, Rect *pClipRect )
{
RGpDrawingSurface		drawingSurface;
R2dTransform			transform;
RIntRect					outRect( pClipRect->left, pClipRect->top, pClipRect->right, pClipRect->bottom );

/**  no graphic  **/
	if ( gpGradientBackdrop == nil )  
		return;

/**  render gradient  **/
	SetCursor( *(GetCursor( watchCursor )) );
	try
	{
		if ( drawingSurface.Initialize( (GrafPtr)pPort ) )
		{
			const RColor&	oldColor = drawingSurface.GetFillColor();
			YPercentage		pctOldPrecision = drawingSurface.GetGradientPrecision();
			gpGradientBackdrop->SetGradientLength( (YRealDimension)outRect.Width() );
			drawingSurface.SetFillColor( *gpGradientBackdrop );
			drawingSurface.FillRectangle( outRect, transform );
			drawingSurface.SetFillColor( oldColor );
		}
	}
	catch ( YException except )
	{
		switch ( except ) 
		{
			case kMemory:
				StopAlert( rLowMemoryALRT, nil );							// out of memeory
				break;
			default:
				AlertOnError( stopIcon, 17000, false );					// generic application file error
				break;
		}
	}
	catch ( ... )
	{
		AlertOnError( stopIcon, 19001, false );							// should not happen
	}
	SetCursor( &qd.arrow );
}


/* ----------------------------------------------------------------------------
	DrawGraphicToPict 
	Create pict drawing surface and render graphic.
	Returns nothing.													 
	---------------------------------------------------------------------------- */
void  DrawGraphicToPict ( WindowPtr pPort )
{
RPictureDrawingSurface	drawingSurface;
R2dTransform				transform;
RIntRect						graphicRect;
RIntRect						graphicOutRect;
RIntSize 					graphicSize;
RIntPoint					ptCenter;

/**  no graphic  **/
	if ( gpGraphic == nil && gpHeadline == nil )  
		return;
		 
/**  setup graphic for output  **/
	if ( gpGraphic == nil )
		graphicSize = gpHeadline->GetGraphicSize();
	else
		graphicSize = ((RSingleGraphic*)gpGraphic)->GetGraphicSize();
	ptCenter.m_x = graphicSize.m_dx / 2;
	ptCenter.m_y = graphicSize.m_dy / 2;
	graphicRect = RIntRect( graphicSize );
	transform.PreRotateAboutPoint( ptCenter.m_x, ptCenter.m_y, DegreesToRadians( gRotateDegrees ) );
	_ApplyUserScale( transform, &graphicRect, drawingSurface.GetDPI() );
	if ( gFlipDirection == kFlipHorizontal )
	{
		transform.PreTranslate( graphicRect.m_Right, 0 );
		transform.PreScale( -1, 1 );
	}
	else if ( gFlipDirection == kFlipVertical )
	{
		transform.PreTranslate( 0, graphicRect.m_Bottom );
		transform.PreScale( 1, -1 );
	}
	else if ( gFlipDirection == kFlipHorzAndVert )
	{
		transform.PreTranslate( graphicRect.m_Right, graphicRect.m_Bottom );
		transform.PreScale( -1, -1 );
	}
	graphicOutRect = _ApplyRotationCompensation( transform, graphicRect, gRotateDegrees );

/**  render graphic  **/
	SetCursor( *(GetCursor( watchCursor )) );
	try
	{
		if ( drawingSurface.Initialize( (GrafPtr)pPort, graphicOutRect ) )
		{
			if ( gpGraphic == nil )  
				gpHeadline->Render( drawingSurface, RRealSize( graphicRect.m_Right, graphicRect.m_Bottom ), transform, RIntRect( -16384, -16384, 16384, 16384 ) );
			else
				gpGraphic->Render( drawingSurface, RRealSize( graphicRect.m_Right, graphicRect.m_Bottom ), transform, RIntRect( -16384, -16384, 16384, 16384 ) );
			drawingSurface.Close();
			drawingSurface.CopyToClipboard();
		}
	}
	catch ( YException except )
	{
		switch ( except ) 
		{
			case kMemory:
				StopAlert( rLowMemoryALRT, nil );							// out of memeory
				break;
			default:
				AlertOnError( stopIcon, 17000, false );					// generic application file error
				break;
		}
	}
	catch ( ... )
	{
		AlertOnError( stopIcon, 19001, false );							// should not happen
	}
	SetCursor( &qd.arrow );
}


/* ----------------------------------------------------------------------------
	CalcGraphicRect 

	Returns 												
	---------------------------------------------------------------------------- */
RRealSize  CalcDefaultHeadlineScreenSize ( )
{
R2dTransform			transform;
RIntRect					graphicRect;

	graphicRect = RIntRect( RHeadlineGraphic::GetDefaultHeadlineSize( gpHeadline->IsHorizontal() ) );
	_ApplyUserScale( transform, &graphicRect, RIntSize( 72, 72 ) );

	return RRealSize( graphicRect.m_Right, graphicRect.m_Bottom );
}

/* ----------------------------------------------------------------------------
	_ApplyUserScroll 
	Set transform to scroll graphic.
	Returns nothing.												
	---------------------------------------------------------------------------- */
static  void  _ApplyUserScroll ( R2dTransform& transform )
{
	transform.PostTranslate( gScroll.m_dx, gScroll.m_dy );
}

/* ----------------------------------------------------------------------------
	_ApplyRotationCompensation
	Adjust transform so that graphic does not rotate off the screen.
	Returns rect of rotated graphic.
	---------------------------------------------------------------------------- */
static  RIntRect  _ApplyRotationCompensation ( R2dTransform& transform, const RIntRect& graphicRect, YAngle rotationAngle )
{
RIntVectorRect			rotatedRect( 0, 0, graphicRect.m_Right, graphicRect.m_Bottom );

	if ( rotationAngle != 0 )
	{
		rotatedRect.RotateAboutCenter( rotationAngle );
		transform.PreTranslate( -rotatedRect.m_TransformedBoundingRect.m_Left, -rotatedRect.m_TransformedBoundingRect.m_Top );
	}

	return RIntRect( 0, 0, rotatedRect.m_TransformedBoundingRect.Width(), rotatedRect.m_TransformedBoundingRect.Height() );
}

/* ----------------------------------------------------------------------------
	_ApplyPrinterScale 
	Set printer to best resolution and resize graphic to match.
	Returns													  
	---------------------------------------------------------------------------- */
static  void  _ApplyPrinterScale ( RIntRect* graphicRect, RPrinterDrawingSurface& drawingSurface, BOOLEAN fUseMaxResolution )
{
RIntSize 		screen = drawingSurface.RGpDrawingSurface::GetDPI();		// screen resolution
RIntSize 		dpi;																		// printer resolution
float				scale;																	// screen to printer scale factor

	if ( fUseMaxResolution )
 		dpi = drawingSurface.GetMaxDPI( TRUE );									// maximum printer resolution
	else
 		dpi = drawingSurface.GetDPI( );												// current printer resolution

	if ( dpi.m_dx != dpi.m_dy || screen.m_dx >= dpi.m_dx || screen.m_dy >= dpi.m_dy )
		return;

	scale = (float)dpi.m_dy / (float)screen.m_dy;								// screen to printer scale factor
 	graphicRect->m_Bottom = ::Round( (float)graphicRect->m_Bottom * scale );
	graphicRect->m_Right = ::Round( (float)graphicRect->m_Right  * scale );
}

/* ----------------------------------------------------------------------------
	_ApplyUserScale 
	Scale graphic per user menu selection.  Assumes a zero based graphic rectangle!
	Returns nothing.												
	---------------------------------------------------------------------------- */
static  void  _ApplyUserScale ( R2dTransform& transform, RIntRect* graphicRect, RIntSize& dpi )
{
	transform.PreScale( (double)dpi.m_dx / (double)kSystemDPI, (double)dpi.m_dy / (double)kSystemDPI ); 
	if ( gpGraphic == nil )  										// scale already applied at construct
		*graphicRect = RIntRect( RHeadlineGraphic::GetDefaultHeadlineSize( gpHeadline->IsHorizontal() ) );
	graphicRect->m_Bottom = ((double)graphicRect->m_Bottom * (double)kSystemDPI) / (double)dpi.m_dy;
	graphicRect->m_Right = ((double)graphicRect->m_Right * (double)kSystemDPI) / (double)dpi.m_dx;
	switch ( gScaleMenuItem ) 
	{
		case k10SMENU:
 			graphicRect->m_Bottom = graphicRect->m_Bottom * 0.10;
			graphicRect->m_Right = graphicRect->m_Right  * 0.10;
			break;
		 case k25SMENU:
			graphicRect->m_Bottom = graphicRect->m_Bottom >> 2;
			graphicRect->m_Right = graphicRect->m_Right  >> 2;
			break;
		 case k50SMENU:
			graphicRect->m_Bottom = graphicRect->m_Bottom >> 1;
			graphicRect->m_Right = graphicRect->m_Right  >> 1;
			break;
		 case k75SMENU:
			graphicRect->m_Bottom = graphicRect->m_Bottom * 0.75;
			graphicRect->m_Right = graphicRect->m_Right  * 0.75;
			break;
		 case k100SMENU:
			break;
		 case k150SMENU:
			graphicRect->m_Bottom = graphicRect->m_Bottom * 1.5;
			graphicRect->m_Right = graphicRect->m_Right * 1.5;
			break;
		 case k200SMENU:
			graphicRect->m_Bottom = graphicRect->m_Bottom << 1;
			graphicRect->m_Right = graphicRect->m_Right << 1;
			break;
		 	case k300SMENU:
			graphicRect->m_Bottom = graphicRect->m_Bottom * 3;
			graphicRect->m_Right = graphicRect->m_Right  * 3;
			break;
		 case k500SMENU:
			graphicRect->m_Bottom = graphicRect->m_Bottom * 5;
			graphicRect->m_Right = graphicRect->m_Right  * 5;
			break;
		 default:
			break;
	 }
}

/* ----------------------------------------------------------------------------
	_fIsPostScript 
	Checks a print record to see if the associated printer is postscript.
	Returns boolean indicating postscript.													 
	---------------------------------------------------------------------------- */
static  BOOLEAN  _fIsPostScript ( THPrint hPrintInfo )
{
	if ( hPrintInfo == nil )
		return false;
	return( (((**hPrintInfo).prStl.wDev & 0x0000FF00L) >> 8) == 0x03 );
//	return( (**hPrintInfo).iPrVersion == kPostScriptPrinterVersion )
}

/* ----------------------------------------------------------------------------
	_DrawPolygon 
	Unused test routine.
	Returns nothing.												
	---------------------------------------------------------------------------- */
static void _DrawPolygon( void )
{
PolyHandle				hPoly;

	if ( (hPoly = ::OpenPoly( )) == nil )
		return;
	::MoveTo( 100, 100 );
	::LineTo( 200, 100 );
	::LineTo( 200, 200 );
	::LineTo( 100, 200 );
	::LineTo( 100, 100 );
	::ClosePoly( );
	::FramePoly( hPoly );
	::KillPoly( hPoly );
}
