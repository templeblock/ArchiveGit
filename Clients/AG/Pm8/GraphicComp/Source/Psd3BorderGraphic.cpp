// *****************************************************************************
//
// File name:			Psd3BorderGraphic.cpp
//
// Project:				Renaissance Framework
//
// Author:				G. Brown
//
// Description:		Definition of the RPsd3BorderGraphic class.
//
// Portability:		Platform-independent
//
// Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
// Client:				Broderbund Software, Inc.
//
// Copyright (c) 1995-1996 Turning Point Software.  All rights reserved.
//
// *****************************************************************************

#include	"GraphicIncludes.h"

ASSERTNAME

#include "Psd3BorderGraphic.h"
#include "RailGraphicUtility.h"
#include "DrawingSurface.h"

// A mutli-part graphic header preceeds the actual graphic data in all 
// types of multi-part graphics.  Although the structure of this header
// is constant for all multi-part graphics, valid entries in the structure
// vary by graphic type.  The following documentation lists the valid
// combinations of flags and graphics for the RPsd3BorderGraphic class,
// defined by EPsd3BorderElement.

//	RPsd3MultiPartGraphicHeader members:
//	uWORD m_PartsList
//		Indicates existence of a particular piece of a multi-part graphic.
//		kPsd3BorderCorner is required.  Combinations of other elements may include:
//
//		Group 1:
//			kPsd3BorderTopConnector
//			kPsd3BorderTopMiddle
//			kPsd3BorderTopRail
//			kPsd3BorderSideConnector
//			kPsd3BorderSideMiddle
//			kPsd3BorderSideRail
//
//		Group 2:
//			kPsd3BorderTopAlt1
//			kPsd3BorderTopAlt2
//			kPsd3BorderSideAlt1
//			kPsd3BorderSideAlt2
//
//		When elements of Group 1 are present, it is not valid to also specify the existence
//		of elements in Group 2, and vice versa.
//
// uWORD m_Draw;
//		Not used by Renaissance.
//
// uWORD m_Flip;
//		Indicates that a mirror image of the graphic is to be displayed instead of the original
//		graphic on opposite sides of the border.  All values of EPsd3BorderElement are valid for
//		this field.  Additionally, kPsd3BorderYCornerFlip is used to indicate that the corner piece
//		is to be reflected across the horizontal axis (a reflection of the corner piece across the
//		vertical access is indicated by kPsd3BorderCorner in this field).
//
// uWORD m_Stretch;	
//		Indicates that a rail graphic is to be stretched between the corner pieces instead
//		of tiled.  The only valid entries for this field are kPsd3BorderTopRail and kPsd3BorderSideRail.
//		Additionally, when these flags are specified for this field, the only valid entries in 
//		m_PartsList are kPsd3BorderCorner, kPsd3BorderTopRail and kPsd3BorderSideRail (i.e. a "stretch"
//		border may consist only of a corner piece and top/bottom and left/right rails).
//
// uWORD m_Unused[24];
//		Not used by Renaissance
//
// The graphics are stored in the following order (provided they exist):
//
//		Corner
//
//		Group 1:
//			Top alt1
//			Top alt2
//			Side alt1
//			Side alt2
//	
//		Group 2:
//			Top connector
//			Top middle
//			Top rail
//			Side connector
//			Side middle
//			Side rail


// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::RPsd3BorderGraphic()
//
// Description:			Constructor
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
RPsd3BorderGraphic::RPsd3BorderGraphic()
:	m_pCorner(NULL),
	m_pHorizontalConnector(NULL),
	m_pHorizontalMiddle(NULL),
	m_pHorizontalRail(NULL),
	m_pHorizontalAlt1(NULL),
	m_pHorizontalAlt2(NULL),
	m_pVerticalConnector(NULL),
	m_pVerticalMiddle(NULL),
	m_pVerticalRail(NULL),
	m_pVerticalAlt1(NULL),
	m_pVerticalAlt2(NULL),
	m_yBorderThickness(kDefaultBorderThickness)
{
}

// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::~RPsd3BorderGraphic()
//
// Description:			Destructor
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
RPsd3BorderGraphic::~RPsd3BorderGraphic()
{
	delete m_pCorner;
	delete m_pHorizontalConnector;
	delete m_pHorizontalAlt1;
	delete m_pHorizontalAlt2;
	delete m_pVerticalConnector;
	delete m_pVerticalAlt1;
	delete m_pVerticalAlt2;

	// If we use the same piece for the entire border, we don't want to delete the corner twice
	if (m_pHorizontalRail != m_pCorner) delete m_pHorizontalRail;
	if (m_pVerticalRail != m_pCorner) delete m_pVerticalRail;

	// If we use the same piece for the middle as we do for Alt1, don't delete the same piece twice
	if (m_pHorizontalMiddle != m_pHorizontalAlt1) delete m_pHorizontalMiddle;
	if (m_pVerticalMiddle != m_pVerticalAlt1) delete m_pVerticalMiddle;
}

// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::Initialize()
//
// Description:			Loads a graphic into memory based on parameters
//
// Returns:					TRUE if successful, FALSE otherwise
//
// Exceptions:				None
//
// ****************************************************************************
BOOLEAN RPsd3BorderGraphic::Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth, BOOLEAN fMonochrome)
{
	// Clean up internals
	Undefine();
	
	// Load border pieces
	RPsd3MultiPartGraphicHeader* pHeader = (RPsd3MultiPartGraphicHeader*) pGraphicData;
	sLONG GraphicOffset = sizeof(RPsd3MultiPartGraphicHeader);

	// byte swap data on mac
#ifdef MAC
	tintSwapStructs( pHeader, sizeof(RPsd3MultiPartGraphicHeader), 1, 0x0000000155L ); 
#endif
	
	// There is always a corner piece
	if (pHeader->m_PartsList & kPsd3BorderCorner)
	{
		const uBYTE* pCurrentGraphic = pGraphicData + GraphicOffset;
		RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
		if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
		{
			delete pGraphic;			
			return FALSE;
		}
		else
		{
			m_pCorner = pGraphic;
			// If corner piece only is given, repeat piece around entire border
			if (pHeader->m_PartsList == kPsd3BorderCorner)
			{
				m_pHorizontalRail = pGraphic;
				m_pVerticalRail = pGraphic;
				return TRUE;
			}
			else GraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
		}
	}
	else
	{
		TpsAssertAlways("Border has no corner.  Cannot load.");
		return FALSE;
	}
	
	if (pHeader->m_PartsList & kPsd3BorderTopAlt1)
	{
		// Next graphics will be in the order:
		//	TopAlt1
		//	TopAlt2
		//	SideAlt1
		//	SideAlt2
		// (if each exists)

		if (pHeader->m_PartsList & kPsd3BorderTopAlt1)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + GraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
			{
				delete pGraphic;			
				return FALSE;
			}
			else
			{
				m_pHorizontalAlt1 = pGraphic;
				GraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
			}
		}

		if (pHeader->m_PartsList & kPsd3BorderTopAlt2)
		{			
			const uBYTE* pCurrentGraphic = pGraphicData + GraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
			{
				delete pGraphic;			
				return FALSE;
			}
			else
			{
				m_pHorizontalAlt2 = pGraphic;
				GraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
			}
		}
		if (pHeader->m_PartsList & kPsd3BorderSideAlt1)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + GraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
			{
				delete pGraphic;			
				return FALSE;
			}
			else
			{
				m_pVerticalAlt1 = pGraphic;
				GraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
			}
		}

		if (pHeader->m_PartsList & kPsd3BorderSideAlt2)
		{			
			const uBYTE* pCurrentGraphic = pGraphicData + GraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
			{
				delete pGraphic;			
				return FALSE;
			}
			else
			{
				m_pVerticalAlt2 = pGraphic;
				GraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
			}
		}
	}
	else
	{
		// Next graphics will be in the order
		//	TopConnector
		//	TopMiddle
		//	TopRail
		//	SideConnector
		//	SideMiddle
		//	SideRail
		// (if each exists)
		if (pHeader->m_PartsList & kPsd3BorderTopConnector)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + GraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
			{
				delete pGraphic;			
				return FALSE;
			}
			else
			{
				m_pHorizontalConnector = pGraphic;
				GraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
			}
		}

		if (pHeader->m_PartsList & kPsd3BorderTopMiddle)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + GraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
			{
				delete pGraphic;			
				return FALSE;
			}
			else
			{
				m_pHorizontalMiddle = pGraphic;
				GraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
			}
		}

		if (pHeader->m_PartsList & kPsd3BorderTopRail)
		{
			TpsAssert(!(m_pHorizontalAlt1 || m_pHorizontalAlt2), "Border specified both horizontal alternating rails and single rail configuration.");
			const uBYTE* pCurrentGraphic = pGraphicData + GraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
			{
				delete pGraphic;			
				return FALSE;
			}
			else
			{
				m_pHorizontalRail = pGraphic;
				GraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
			}
		}

		if (pHeader->m_PartsList & kPsd3BorderSideConnector)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + GraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
			{
				delete pGraphic;			
				return FALSE;
			}
			else
			{
				m_pVerticalConnector = pGraphic;
				GraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
			}
		}

		if (pHeader->m_PartsList & kPsd3BorderSideMiddle)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + GraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
			{
				delete pGraphic;			
				return FALSE;
			}
			else
			{
				m_pVerticalMiddle = pGraphic;
				GraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
			}
		}

		if (pHeader->m_PartsList & kPsd3BorderSideRail)
		{
			TpsAssert(!(m_pVerticalAlt1 || m_pVerticalAlt2), "Border specified both vertical alternating rails and single rail configurations.");
			const uBYTE* pCurrentGraphic = pGraphicData + GraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
			{
				delete pGraphic;			
				return FALSE;
			}
			else
			{
				m_pVerticalRail = pGraphic;
				GraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
			}
		}
	}

	// Make sure we have both alternating rails
	if (m_pHorizontalAlt1 || m_pHorizontalAlt2) TpsAssert(m_pHorizontalAlt1 && m_pHorizontalAlt2, "Not a valid rail arrangement.");
	if (m_pVerticalAlt1 || m_pVerticalAlt2) TpsAssert(m_pVerticalAlt1 && m_pVerticalAlt2, "Not a valid rail arrangement.");


	// Mirror flags
	// NOTE Most of these are always FALSE because Broderbund's file format does not support them
	m_fMirrorCorner =						(BOOLEAN)((pHeader->m_Flip & kPsd3BorderCorner) ? TRUE : FALSE);	
	m_fMirrorHorizontalConnector =	TRUE;
	m_fMirrorHorizontalRail =			(BOOLEAN)((pHeader->m_Flip & kPsd3BorderTopRailFlip) ? TRUE : FALSE);
	m_fMirrorHorizontalAlt1 =			FALSE;
	m_fMirrorHorizontalAlt2 =			FALSE;
	m_fMirrorVerticalConnector =		TRUE;
	m_fMirrorVerticalRail =				(BOOLEAN)((pHeader->m_Flip & kPsd3BorderSideRailFlip) ? TRUE : FALSE);
	m_fMirrorVerticalAlt1 =				FALSE;
	m_fMirrorVerticalAlt2 =				FALSE;

	// Invert flags
	m_fInvertCorner =						(BOOLEAN)((pHeader->m_Flip & kPsd3BorderYCornerFlip) ? TRUE : FALSE);
	m_fInvertHorizontalConnector =	(BOOLEAN)((pHeader->m_Flip & kPsd3BorderTopConnector) ? TRUE : FALSE);
	m_fInvertHorizontalRail =			(BOOLEAN)((pHeader->m_Flip & kPsd3BorderTopRail) ? TRUE : FALSE);	
	m_fInvertHorizontalAlt1 =			(BOOLEAN)((pHeader->m_Flip & kPsd3BorderTopAlt1) ? TRUE : FALSE);
	m_fInvertHorizontalAlt2 =			(BOOLEAN)((pHeader->m_Flip & kPsd3BorderTopAlt2) ? TRUE : FALSE);
	m_fInvertVerticalConnector =		(BOOLEAN)((pHeader->m_Flip & kPsd3BorderSideConnector) ? TRUE : FALSE);
	m_fInvertVerticalRail =				(BOOLEAN)((pHeader->m_Flip & kPsd3BorderSideRail) ? TRUE : FALSE);	
	m_fInvertVerticalAlt1 =				(BOOLEAN)((pHeader->m_Flip & kPsd3BorderSideAlt1) ? TRUE : FALSE);
	m_fInvertVerticalAlt2 =				(BOOLEAN)((pHeader->m_Flip & kPsd3BorderSideAlt2) ? TRUE : FALSE);

	m_fInvertHorizontalMiddle =		(BOOLEAN)((pHeader->m_Flip & kPsd3BorderTopMiddle) ? TRUE : FALSE);	
	m_fInvertVerticalMiddle =			(BOOLEAN)((pHeader->m_Flip & kPsd3BorderSideMiddle) ? TRUE : FALSE);

	m_fStretchHorizontalRail =			(BOOLEAN)((pHeader->m_Stretch & kPsd3BorderTopRail) ? TRUE : FALSE);
	m_fStretchVerticalRail =			(BOOLEAN)((pHeader->m_Stretch & kPsd3BorderSideRail) ? TRUE : FALSE);
	
	// If we are using alternating rails, set the first alternate as a middle
	// piece so we have an odd number of rails (A is Alt1 and the middle):
	//		A = Alt1
	//		B = Alt2
	//		ABAB A BABA
	if (m_pHorizontalAlt1 && m_pHorizontalAlt2)
	{
		m_pHorizontalMiddle = m_pHorizontalAlt1;
		m_fInvertHorizontalMiddle = m_fInvertHorizontalAlt1;
	}
	if (m_pVerticalAlt1 && m_pVerticalAlt2)
	{
		m_pVerticalMiddle = m_pVerticalAlt1;
		m_fInvertVerticalMiddle = m_fInvertVerticalAlt1;
	}

	// Enable the render cache on the graphics which comprise the border

	//	If we don't mirror, we can image two consectutive pieces, let it use the render cache
	if ( !m_fMirrorCorner )
		InitializeRenderCache(m_pCorner);
//	Since these ALWAYS mirror, they will have to render 4 times, don't use render cache.
//	InitializeRenderCache(m_pHorizontalConnector);
//	InitializeRenderCache(m_pVerticalConnector);

	InitializeRenderCache(m_pHorizontalMiddle);
	InitializeRenderCache(m_pHorizontalRail);
	InitializeRenderCache(m_pHorizontalAlt1);
	InitializeRenderCache(m_pHorizontalAlt2);
	InitializeRenderCache(m_pVerticalMiddle);
	InitializeRenderCache(m_pVerticalRail);
	InitializeRenderCache(m_pVerticalAlt1);
	InitializeRenderCache(m_pVerticalAlt2);

	return TRUE;
}

// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::ContainsGradientData()
//
// Description:			Returns TRUE if this graphic contains gradient data
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
BOOLEAN RPsd3BorderGraphic::ContainsGradientData()
{	
	if (	m_pCorner && m_pCorner->ContainsGradientData() ||
			m_pHorizontalConnector && m_pHorizontalConnector->ContainsGradientData() ||
			m_pHorizontalMiddle && m_pHorizontalMiddle->ContainsGradientData() ||
			m_pHorizontalRail && m_pHorizontalRail->ContainsGradientData() ||
			m_pHorizontalAlt1 && m_pHorizontalAlt1->ContainsGradientData() ||
			m_pHorizontalAlt2 && m_pHorizontalAlt2->ContainsGradientData() ||
			m_pVerticalConnector && m_pVerticalConnector->ContainsGradientData() ||
			m_pVerticalMiddle && m_pVerticalMiddle->ContainsGradientData() ||
			m_pVerticalRail && m_pVerticalRail->ContainsGradientData() ||
			m_pVerticalAlt1 && m_pVerticalAlt1->ContainsGradientData() ||
			m_pVerticalAlt2 && m_pVerticalAlt2->ContainsGradientData())		
		return TRUE;
	else
		return FALSE;
}

// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::Render()
//
// Description:			Draws the border
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3BorderGraphic::Render(RDrawingSurface& rDrawingSurface, const RRealSize& rBorderSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor, BOOLEAN)
{
	Render(&rDrawingSurface, NULL, rBorderSize, rTransform, rRender, rMonoColor);
}

// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::Render()
//
// Description:			Draws the border
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3BorderGraphic::Render(RDrawingSurface* pDrawingSurface, RPath* pPath, const RRealSize& rBorderSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor)
{	
	if (!m_pCorner)
	{
		TpsAssertAlways("Border has no corners.  Can't render.");
		return;
	}

	//	Compute the inverse of the transform 
	R2dTransform	invTransform( rTransform );
	invTransform.Invert( );

	// Compute graphic sizes
	RRealSize rCornerSize(m_yBorderThickness, m_yBorderThickness);

	//	Adjust the graphic size to be a whole size according to the device (rounded down)
	rCornerSize	*= rTransform;
	rCornerSize.m_dx	= floor( rCornerSize.m_dx );
	rCornerSize.m_dy	= floor( rCornerSize.m_dy );

	//	If we have a border size that is less than kMinimumDevicePixelsToRender, we don't render
	const	YRealDimension	kMinimumDevicePixelsToRender	= 1;
	if ( (::Abs(rCornerSize.m_dx) <= kMinimumDevicePixelsToRender) && (::Abs(rCornerSize.m_dy) <= kMinimumDevicePixelsToRender) )
		return;

	//	Convert back to Renaissance Units
	rCornerSize	*= invTransform;

	// Rendering the border can take up to seconds on a P166, so we check how long the corners
	// take in order to make a decision on using the wait cursor. We can't just use the wait
	// cursor because the cursor will switch (ugly...) while moving in the border since this
	// code it executed whenever the cursor touchs the border.
	HCURSOR oldCursor = NULL;
	DWORD n1 = ::GetTickCount();	

	// Corners	
	RenderCorners(m_pCorner, rCornerSize, m_fMirrorCorner, m_fInvertCorner, pDrawingSurface, pPath, rBorderSize, rTransform, rRender, rMonoColor);
	DWORD n2 = ::GetTickCount();
	if(n2 - n1 > 100) // n ticks is a very long time, post the wait cursor.
	{
		oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));	
	}

	//	Render the horizontal portions of the border
	RRealSize	rHorizSize( rBorderSize.m_dx - 2.0 * rCornerSize.m_dx, rBorderSize.m_dy );
	RenderHorizontal(pDrawingSurface, pPath, rHorizSize, rTransform, rRender, rMonoColor );

	//	Render the vertical portions of the border
	RRealSize	rVertSize( rBorderSize.m_dx, rBorderSize.m_dy - 2.0 * rCornerSize.m_dy );
	RenderVertical(pDrawingSurface, pPath, rVertSize, rTransform, rRender, rMonoColor );

	if(oldCursor)
	{
		SetCursor(oldCursor);
	}
}

// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::RenderHorizontal()
//
// Description:			Draws the Horizontal sections of the borders
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3BorderGraphic::RenderHorizontal(RDrawingSurface* pDrawingSurface, RPath* pPath, const RRealSize& rHorizSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor )
{
	RIntVectorRect	rOutputRect;
	RRealSize		rRenderRailSize1;
	RRealSize		rRenderRailSize2;
	BOOLEAN			fIsPrinting	= (pDrawingSurface && pDrawingSurface->IsPrinting())? TRUE : FALSE;

	//	Compute the inverse of the transform
	R2dTransform	invTransform( rTransform );
	invTransform.Invert( );

	// Compute graphic sizes - based on the corner size (rounded down)
	RRealSize rCornerSize(m_yBorderThickness, m_yBorderThickness);
	rCornerSize			*= rTransform;
	rCornerSize.m_dx	= floor( rCornerSize.m_dx );
	rCornerSize.m_dy	= floor( rCornerSize.m_dy );
	rCornerSize			*= invTransform;

	R2dTransform	rHorizontalTransform( rTransform );
	rHorizontalTransform.PreTranslate(rCornerSize.m_dx, 0.0);

	RRealSize rHorizontalBoxSize( rHorizSize );
	RRealSize rHorizontalMiddleSize( ::GetXSize(m_pHorizontalMiddle, m_yBorderThickness) );
	rHorizontalMiddleSize		*= rTransform;
	rHorizontalMiddleSize.m_dx	= floor( rHorizontalMiddleSize.m_dx );
	rHorizontalMiddleSize.m_dy	= floor( rHorizontalMiddleSize.m_dy );
	rHorizontalMiddleSize		*= invTransform;

	// Horizontal connectors
	if (m_pHorizontalConnector)
	{
		RRealSize rHorizontalConnectorSize =	::GetXSize(m_pHorizontalConnector, m_yBorderThickness);
		rHorizontalConnectorSize		*= rTransform;
		rHorizontalConnectorSize.m_dx	= floor( rHorizontalConnectorSize.m_dx );
		rHorizontalConnectorSize.m_dy	= floor( rHorizontalConnectorSize.m_dy );
		rHorizontalConnectorSize		*= invTransform;
		RenderCorners(m_pHorizontalConnector, rHorizontalConnectorSize, m_fMirrorHorizontalConnector, m_fInvertHorizontalConnector, pDrawingSurface, pPath, rHorizontalBoxSize, rHorizontalTransform, rRender, rMonoColor);
		UpdateX(rHorizontalConnectorSize, rHorizontalBoxSize, rHorizontalTransform);
	}

	//	If there are some sort of rails....
	if ( m_pHorizontalRail || (m_pHorizontalAlt1 && m_pHorizontalAlt2) )
	{

		//	Check for the special case where we are stretching the rail
		if ( m_pHorizontalRail && m_fStretchHorizontalRail )
		{
			// Note: This is a very rare flag.  As of 12/6/96, there is only one
			// graphic which uses it: Wooden Boards.
			//	5-8-98: There are now middle pieces in some of the NEW stretched borders...
			RRealSize rStretchedRailSize(rHorizontalBoxSize.m_dx, m_yBorderThickness);
			//	If there is a middle piece, the StretchedRailSize is only 1/2 box size - middle size
			if ( m_pHorizontalMiddle )
				rStretchedRailSize.m_dx	= (rStretchedRailSize.m_dx - rHorizontalMiddleSize.m_dx) / 2;

			//	Render the stretched rail
			RenderCorners(m_pHorizontalRail, rStretchedRailSize, FALSE, m_fInvertHorizontalRail, pDrawingSurface, pPath, rHorizontalBoxSize, rHorizontalTransform, rRender, rMonoColor);

			//	Adjust the box size for the middle section to render correctly
			rHorizontalBoxSize.m_dx -= 2.0 * rStretchedRailSize.m_dx;
			rHorizontalTransform.PreTranslate( rStretchedRailSize.m_dx, 0.0 );
		}
		else
		{
			//	Here we have to perform some sort of tiling.
			sLONG			sRail;
			sLONG			sRailCount;

			YRealDimension	xScale;
			YRealDimension	yScale;
			YAngle			rotation;
			rTransform.Decompose( rotation, xScale, yScale );

			//	Compute some useful values for later use
			RRealSize rHorizontalRailSize = ::GetXSize(m_pHorizontalRail, m_yBorderThickness);
			RRealSize rHorizontalAlt1Size = ::GetXSize(m_pHorizontalAlt1, m_yBorderThickness);
			RRealSize rHorizontalAlt2Size = ::GetXSize(m_pHorizontalAlt2, m_yBorderThickness);

			YRealDimension yVariableLength = (rHorizontalBoxSize.m_dx - rHorizontalMiddleSize.m_dx) / 2.0;
			YRealDimension	yNominalDimension	=  (m_pHorizontalRail)? rHorizontalRailSize.m_dx : rHorizontalAlt1Size.m_dx+rHorizontalAlt2Size.m_dx;
			sRailCount = ::GetRailCount( yVariableLength, yNominalDimension );

			if (sRailCount > 0)
			{
				//	What is the excess
				YIntDimension	smallSize		= sRailCount;
				YIntDimension	increment		= 0;
				YRealDimension	xStartOffset	= 0.0;
				YIntDimension	excessCounter	= 0;
				R2dTransform	renderTransform;
				RRealSize		renderSize;
		
				if ( m_pHorizontalRail )
				{
					RRealSize	rRailSize( rHorizontalRailSize );
					// Compute the new rail sizes (the small and large ones)
					RRealSize	smallRailSize( yVariableLength / (YFloatType)sRailCount, rHorizontalRailSize.m_dy );
					smallRailSize	*= rTransform;
					smallRailSize	= RRealSize( floor( smallRailSize.m_dx ), floor( smallRailSize.m_dy ) );
					RRealSize	largeRailSize( smallRailSize.m_dx+1.0, smallRailSize.m_dy );
					//
					//	Re-expand to logical units
					smallRailSize	*= invTransform;
					largeRailSize	*= invTransform;
					//
					//	Reset the y dimension
					smallRailSize.m_dy	= largeRailSize.m_dy = rCornerSize.m_dy;
					increment				= static_cast<YIntDimension>( (yVariableLength - (smallRailSize.m_dx * sRailCount)) * xScale) + 1;	

					//	Render the TopLeft Rail
					renderTransform	= rHorizontalTransform;
					renderSize			= rHorizontalBoxSize;
					excessCounter		= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Add the extra to the excessCounter
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rRailSize		= largeRailSize;
							excessCounter	-= smallSize;
						}
						else
							rRailSize	= smallRailSize;

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rRailSize;
						}
						else
						{
							rRenderRailSize1	= largeRailSize;
						}

						rOutputRect	= RIntRect(RIntSize(rRailSize)) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pHorizontalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pHorizontalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
						renderSize.m_dx -= 2.0 * rRailSize.m_dx;
						renderTransform.PreTranslate( rRailSize.m_dx, 0.0 );
					}

 					//	Render the last piece to ensure that there is complete coverage..
					rRailSize.m_dx	= (renderSize.m_dx - rHorizontalMiddleSize.m_dx) / 2.0;
					if ( fIsPrinting )
						rRenderRailSize1	= rRailSize;
					else
						rRenderRailSize1	= largeRailSize;
					rOutputRect	= RIntRect(RIntSize(rRailSize)) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pHorizontalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pHorizontalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
					renderSize.m_dx -= 2.0 * rRailSize.m_dx;
					renderTransform.PreTranslate( rRailSize.m_dx, 0.0 );

					//	Render the bottom left rail
					renderTransform	= rHorizontalTransform;
					::FlipY(rHorizontalBoxSize, renderTransform);
					renderSize			= rHorizontalBoxSize;
					excessCounter		= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Add the extra to the excessCounter
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rRailSize		= largeRailSize;
							excessCounter	-= smallSize;
						}
						else
							rRailSize	= smallRailSize;

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rRailSize;
						}
						else
						{
							rRenderRailSize1	= largeRailSize;
						}

						if (!m_fInvertHorizontalRail) ::FlipY( rRailSize, renderTransform );
						rOutputRect	= RIntRect(RIntSize(rRailSize)) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pHorizontalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pHorizontalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
						if (!m_fInvertHorizontalRail) ::FlipY( rRailSize, renderTransform );
						renderSize.m_dx -= 2.0 * rRailSize.m_dx;
						renderTransform.PreTranslate( rRailSize.m_dx, 0.0 );
					}
 					//	Render the last piece to ensure that there is complete coverage..
					rRailSize.m_dx	= (renderSize.m_dx - rHorizontalMiddleSize.m_dx) / 2.0;
					if ( fIsPrinting )
						rRenderRailSize1	= rRailSize;
					else
						rRenderRailSize1	= largeRailSize;
					if (!m_fInvertHorizontalRail) ::FlipY( rRailSize, renderTransform );
					rOutputRect	= RIntRect(RIntSize(rRailSize)) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pHorizontalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pHorizontalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
					if (!m_fInvertHorizontalRail) ::FlipY( rRailSize, renderTransform );
					renderSize.m_dx -= 2.0 * rRailSize.m_dx;
					renderTransform.PreTranslate( rRailSize.m_dx, 0.0 );

					//	Render the TopRight rail
					renderTransform	= rHorizontalTransform;
					::FlipX( rHorizontalBoxSize, renderTransform );
					renderSize			= rHorizontalBoxSize;
					excessCounter		= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Add the extra to the excessCounter
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rRailSize		= largeRailSize;
							excessCounter	-= smallSize;
						}
						else
							rRailSize	= smallRailSize;

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rRailSize;
						}
						else
						{
							rRenderRailSize1	= largeRailSize;
						}

						if (!m_fMirrorHorizontalRail) ::FlipX( rRailSize, renderTransform );
						rOutputRect	= RIntRect(RIntSize(rRailSize)) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pHorizontalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pHorizontalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
						if (!m_fMirrorHorizontalRail) ::FlipX( rRailSize, renderTransform );
						renderSize.m_dx -= 2.0 * rRailSize.m_dx;
						renderTransform.PreTranslate( rRailSize.m_dx, 0.0 );
					}
 					//	Render the last piece to ensure that there is complete coverage..
					rRailSize.m_dx	= (renderSize.m_dx - rHorizontalMiddleSize.m_dx) / 2.0;
					if ( fIsPrinting )
						rRenderRailSize1	= rRailSize;
					else
						rRenderRailSize1	= largeRailSize;
					if (!m_fMirrorHorizontalRail) ::FlipX( rRailSize, renderTransform );
					rOutputRect	= RIntRect(RIntSize(rRailSize)) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pHorizontalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pHorizontalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
					if (!m_fMirrorHorizontalRail) ::FlipX( rRailSize, renderTransform );
					renderSize.m_dx -= 2.0 * rRailSize.m_dx;
					renderTransform.PreTranslate( rRailSize.m_dx, 0.0 );

					//	Render the Bottom Right Rail
					renderTransform	= rHorizontalTransform;
					::FlipY(rHorizontalBoxSize, renderTransform);
					::FlipX(rHorizontalBoxSize, renderTransform);
					renderSize			= rHorizontalBoxSize;
					excessCounter		= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Add the extra to the excessCounter
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rRailSize		= largeRailSize;
							excessCounter	-= smallSize;
						}
						else
							rRailSize	= smallRailSize;

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rRailSize;
						}
						else
						{
							rRenderRailSize1	= largeRailSize;
						}

						if (!m_fInvertHorizontalRail) ::FlipY( rRailSize, renderTransform );
						if (!m_fMirrorHorizontalRail) ::FlipX( rRailSize, renderTransform );
						rOutputRect	= RIntRect(RIntSize(rRailSize)) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pHorizontalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pHorizontalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
						if (!m_fMirrorHorizontalRail) ::FlipX( rRailSize, renderTransform );
						if (!m_fInvertHorizontalRail) ::FlipY( rRailSize, renderTransform );
						renderSize.m_dx -= 2.0 * rRailSize.m_dx;
						renderTransform.PreTranslate( rRailSize.m_dx, 0.0 );
					}
 					//	Render the last piece to ensure that there is complete coverage..
					rRailSize.m_dx	= (renderSize.m_dx - rHorizontalMiddleSize.m_dx) / 2.0;
					if ( fIsPrinting )
						rRenderRailSize1	= rRailSize;
					else
						rRenderRailSize1	= largeRailSize;
					if (!m_fInvertHorizontalRail) ::FlipY( rRailSize, renderTransform );
					if (!m_fMirrorHorizontalRail) ::FlipX( rRailSize, renderTransform );
					rOutputRect	= RIntRect(RIntSize(rRailSize)) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pHorizontalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pHorizontalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
					if (!m_fMirrorHorizontalRail) ::FlipX( rRailSize, renderTransform );
					if (!m_fInvertHorizontalRail) ::FlipY( rRailSize, renderTransform );
					renderSize.m_dx -= 2.0 * rRailSize.m_dx;
					renderTransform.PreTranslate( rRailSize.m_dx, 0.0 );

					rHorizontalBoxSize	= renderSize;
					rHorizontalTransform	= renderTransform;
				}
				else	//	must be if (m_pHorizontalAlt1 && m_pHorizontalAlt2)
				{
					RRealSize		rAlt1RailSize( rHorizontalAlt1Size );
					RRealSize		rAlt2RailSize( rHorizontalAlt2Size );
					R2dTransform	renderTransform2;

					// Compute the new rail sizes (the small and large ones)
					YRealDimension	tileDimension( yVariableLength / (YFloatType)sRailCount / yNominalDimension );
					RRealSize		smallAlt1RailSize( tileDimension * rHorizontalAlt1Size.m_dx, rHorizontalAlt1Size.m_dy );
					RRealSize		smallAlt2RailSize( tileDimension * rHorizontalAlt2Size.m_dx, rHorizontalAlt2Size.m_dy );
					smallAlt1RailSize	*= rTransform;
					smallAlt2RailSize	*= rTransform;
					smallAlt1RailSize	= RRealSize( floor( smallAlt1RailSize.m_dx ), floor( smallAlt1RailSize.m_dy ) );
					smallAlt2RailSize	= RRealSize( floor( smallAlt2RailSize.m_dx ), floor( smallAlt2RailSize.m_dy ) );
					RRealSize		largeAlt1RailSize( smallAlt1RailSize.m_dx+1.0, smallAlt1RailSize.m_dy );
					RRealSize		largeAlt2RailSize( smallAlt1RailSize.m_dx+1.0, smallAlt1RailSize.m_dy );
					//
					//	Re-expand to logical units
					smallAlt1RailSize	*= invTransform;
					smallAlt2RailSize	*= invTransform;
					largeAlt1RailSize	*= invTransform;
					largeAlt2RailSize	*= invTransform;
					//
					//	Reset the y dimension
					smallAlt1RailSize.m_dy = largeAlt1RailSize.m_dy = rCornerSize.m_dy;
					smallAlt2RailSize.m_dy = largeAlt2RailSize.m_dy = rCornerSize.m_dy;
					increment					= static_cast<YIntDimension>( (yVariableLength - ((smallAlt1RailSize.m_dx+smallAlt2RailSize.m_dx) * sRailCount)) * xScale) + 1;

					//	Render the TopLeft Horizontal Tiles
					renderTransform	= rHorizontalTransform;
					renderSize			= rHorizontalBoxSize;
					excessCounter		= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Setup the RailSize...
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rAlt1RailSize	= largeAlt1RailSize;
							rAlt2RailSize	= largeAlt2RailSize;
							excessCounter	-= smallSize;
						}
						else
						{
							rAlt1RailSize	= smallAlt1RailSize;
							rAlt2RailSize	= smallAlt2RailSize;
						}

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rAlt1RailSize;
							rRenderRailSize2	= rAlt2RailSize;
						}
						else
						{
							rRenderRailSize1	= largeAlt1RailSize;
							rRenderRailSize2	= largeAlt2RailSize;
						}

						rOutputRect	= RIntRect(RIntSize(rAlt1RailSize)) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pHorizontalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pHorizontalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
						renderSize.m_dx -= 2.0 * rAlt1RailSize.m_dx;
						renderTransform.PreTranslate( rAlt1RailSize.m_dx, 0.0 );
						rOutputRect	= RIntRect(RIntSize(rAlt2RailSize)) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pHorizontalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
						if (pPath) m_pHorizontalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
						renderSize.m_dx -= 2.0 * rAlt2RailSize.m_dx;
						renderTransform.PreTranslate( rAlt2RailSize.m_dx, 0.0 );
					}

					//	Render the last piece to ensure that there is complete coverage..
					tileDimension			= ( renderSize.m_dx - rHorizontalMiddleSize.m_dx ) / 2.0 / yNominalDimension;
					rAlt1RailSize.m_dx	= tileDimension * rHorizontalAlt1Size.m_dx;
					rAlt2RailSize.m_dx	= tileDimension * rHorizontalAlt2Size.m_dx;
					if ( fIsPrinting )
					{
						rRenderRailSize1	= rAlt1RailSize;
						rRenderRailSize2	= rAlt2RailSize;
					}
					else
					{
						rRenderRailSize1	= largeAlt1RailSize;
						rRenderRailSize2	= largeAlt2RailSize;
					}
					rOutputRect	= RIntRect(RIntSize(rAlt1RailSize)) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pHorizontalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pHorizontalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
					renderSize.m_dx -= 2.0 * rAlt1RailSize.m_dx;
					renderTransform.PreTranslate( rAlt1RailSize.m_dx, 0.0 );
					rOutputRect	= RIntRect(RIntSize(rAlt2RailSize)) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pHorizontalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
					if (pPath) m_pHorizontalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
					renderSize.m_dx -= 2.0 * rAlt2RailSize.m_dx;
					renderTransform.PreTranslate( rAlt2RailSize.m_dx, 0.0 );

					//	Render the Bottom Left ( first adjust the transform )
					renderTransform	= rHorizontalTransform;
					::FlipY(rHorizontalBoxSize, renderTransform);
					renderSize			= rHorizontalBoxSize;
					excessCounter		= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Setup the RailSize...
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rAlt1RailSize	= largeAlt1RailSize;
							rAlt2RailSize	= largeAlt2RailSize;
							excessCounter	-= smallSize;
						}
						else
						{
							rAlt1RailSize	= smallAlt1RailSize;
							rAlt2RailSize	= smallAlt2RailSize;
						}

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rAlt1RailSize;
							rRenderRailSize2	= rAlt2RailSize;
						}
						else
						{
							rRenderRailSize1	= largeAlt1RailSize;
							rRenderRailSize2	= largeAlt2RailSize;
						}

						if (!m_fInvertHorizontalAlt1) ::FlipY(rAlt1RailSize, renderTransform);
						rOutputRect	= RIntRect(RIntSize(rAlt1RailSize)) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pHorizontalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pHorizontalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
						if (!m_fInvertHorizontalAlt1) ::FlipY(rAlt1RailSize, renderTransform);
						renderSize.m_dx -= 2.0 * rAlt1RailSize.m_dx;
						renderTransform.PreTranslate( rAlt1RailSize.m_dx, 0.0 );
						if (!m_fInvertHorizontalAlt2) ::FlipY(rAlt2RailSize, renderTransform);
						rOutputRect	= RIntRect(RIntSize(rAlt2RailSize)) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pHorizontalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
						if (pPath) m_pHorizontalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
						if (!m_fInvertHorizontalAlt2) ::FlipY(rAlt2RailSize, renderTransform);
						renderSize.m_dx -= 2.0 * rAlt2RailSize.m_dx;
						renderTransform.PreTranslate( rAlt2RailSize.m_dx, 0.0 );
					}

					//	Render the last piece to ensure that there is complete coverage..
					tileDimension			= ( renderSize.m_dx - rHorizontalMiddleSize.m_dx ) / 2.0 / yNominalDimension;
					rAlt1RailSize.m_dx	= tileDimension * rHorizontalAlt1Size.m_dx;
					rAlt2RailSize.m_dx	= tileDimension * rHorizontalAlt2Size.m_dx;
					if ( fIsPrinting )
					{
						rRenderRailSize1	= rAlt1RailSize;
						rRenderRailSize2	= rAlt2RailSize;
					}
					else
					{
						rRenderRailSize1	= largeAlt1RailSize;
						rRenderRailSize2	= largeAlt2RailSize;
					}
					if (!m_fInvertHorizontalAlt1) ::FlipY(rAlt1RailSize, renderTransform);
					rOutputRect	= RIntRect(RIntSize(rAlt1RailSize)) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pHorizontalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pHorizontalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
					if (!m_fInvertHorizontalAlt1) ::FlipY(rAlt1RailSize, renderTransform);
					renderSize.m_dx -= 2.0 * rAlt1RailSize.m_dx;
					renderTransform.PreTranslate( rAlt1RailSize.m_dx, 0.0 );
					if (!m_fInvertHorizontalAlt2) ::FlipY(rAlt2RailSize, renderTransform);
					rOutputRect	= RIntRect(RIntSize(rAlt2RailSize)) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pHorizontalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
					if (pPath) m_pHorizontalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
					if (!m_fInvertHorizontalAlt2) ::FlipY(rAlt2RailSize, renderTransform);
					renderSize.m_dx -= 2.0 * rAlt2RailSize.m_dx;
					renderTransform.PreTranslate( rAlt2RailSize.m_dx, 0.0 );

					//	Render the Top Right Horizontal Tiles
					renderTransform	= rHorizontalTransform;
					::FlipX(rHorizontalBoxSize, renderTransform);
					renderSize			= rHorizontalBoxSize;
					excessCounter		= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Setup the RailSize...
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rAlt1RailSize	= largeAlt1RailSize;
							rAlt2RailSize	= largeAlt2RailSize;
							excessCounter	-= smallSize;
						}
						else
						{
							rAlt1RailSize	= smallAlt1RailSize;
							rAlt2RailSize	= smallAlt2RailSize;
						}

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rAlt1RailSize;
							rRenderRailSize2	= rAlt2RailSize;
						}
						else
						{
							rRenderRailSize1	= largeAlt1RailSize;
							rRenderRailSize2	= largeAlt2RailSize;
						}

						if (!m_fMirrorHorizontalAlt1) ::FlipX(rAlt1RailSize, renderTransform);
						rOutputRect	= RIntRect(RIntSize(rAlt1RailSize)) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pHorizontalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pHorizontalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
						if (!m_fMirrorHorizontalAlt1) ::FlipX(rAlt1RailSize, renderTransform);
						renderSize.m_dx -= 2.0 * rAlt1RailSize.m_dx;
						renderTransform.PreTranslate( rAlt1RailSize.m_dx, 0.0 );
						if (!m_fMirrorHorizontalAlt2) ::FlipX(rAlt2RailSize, renderTransform);
						rOutputRect	= RIntRect(RIntSize(rAlt2RailSize)) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pHorizontalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
						if (pPath) m_pHorizontalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
						if (!m_fMirrorHorizontalAlt1)	::FlipX(rAlt2RailSize, renderTransform);
						renderSize.m_dx -= 2.0 * rAlt2RailSize.m_dx;
						renderTransform.PreTranslate( rAlt2RailSize.m_dx, 0.0 );
					}

					//	Render the last piece to ensure that there is complete coverage..
					tileDimension			= ( renderSize.m_dx - rHorizontalMiddleSize.m_dx ) / 2.0 / yNominalDimension;
					rAlt1RailSize.m_dx	= tileDimension * rHorizontalAlt1Size.m_dx;
					rAlt2RailSize.m_dx	= tileDimension * rHorizontalAlt2Size.m_dx;
					if ( fIsPrinting )
					{
						rRenderRailSize1	= rAlt1RailSize;
						rRenderRailSize2	= rAlt2RailSize;
					}
					else
					{
						rRenderRailSize1	= largeAlt1RailSize;
						rRenderRailSize2	= largeAlt2RailSize;
					}
					if (!m_fMirrorHorizontalAlt1) ::FlipX(rAlt1RailSize, renderTransform);
					rOutputRect	= RIntRect(RIntSize(rAlt1RailSize)) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pHorizontalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pHorizontalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
					if (!m_fMirrorHorizontalAlt1) ::FlipX(rAlt1RailSize, renderTransform);
					renderSize.m_dx -= 2.0 * rAlt1RailSize.m_dx;
					renderTransform.PreTranslate( rAlt1RailSize.m_dx, 0.0 );
					if (!m_fMirrorHorizontalAlt2) ::FlipX(rAlt1RailSize, renderTransform);
					rOutputRect	= RIntRect(RIntSize(rAlt2RailSize)) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pHorizontalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
					if (pPath) m_pHorizontalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
					if (!m_fMirrorHorizontalAlt2) ::FlipX(rAlt1RailSize, renderTransform);
					renderSize.m_dx -= 2.0 * rAlt2RailSize.m_dx;
					renderTransform.PreTranslate( rAlt2RailSize.m_dx, 0.0 );

					//	Render the Bottom Right ( first adjust the transform )
					renderTransform	= rHorizontalTransform;
					::FlipY(rHorizontalBoxSize, renderTransform);
					::FlipX(rHorizontalBoxSize, renderTransform);
					renderSize			= rHorizontalBoxSize;
					excessCounter		= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Setup the RailSize...
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rAlt1RailSize	= largeAlt1RailSize;
							rAlt2RailSize	= largeAlt2RailSize;
							excessCounter	-= smallSize;
						}
						else
						{
							rAlt1RailSize	= smallAlt1RailSize;
							rAlt2RailSize	= smallAlt2RailSize;
						}

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rAlt1RailSize;
							rRenderRailSize2	= rAlt2RailSize;
						}
						else
						{
							rRenderRailSize1	= largeAlt1RailSize;
							rRenderRailSize2	= largeAlt2RailSize;
						}

						if (!m_fInvertHorizontalAlt1) ::FlipY(rAlt1RailSize, renderTransform);
						if (!m_fMirrorHorizontalAlt1) ::FlipX(rAlt1RailSize, renderTransform);
						rOutputRect	= RIntRect(RIntSize(rAlt1RailSize)) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pHorizontalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pHorizontalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
						if (!m_fMirrorHorizontalAlt1) ::FlipX(rAlt1RailSize, renderTransform);
						if (!m_fInvertHorizontalAlt1) ::FlipY(rAlt1RailSize, renderTransform);
						renderSize.m_dx -= 2.0 * rAlt1RailSize.m_dx;
						renderTransform.PreTranslate( rAlt1RailSize.m_dx, 0.0 );
						if (!m_fInvertHorizontalAlt2) ::FlipY(rAlt2RailSize, renderTransform);
						if (!m_fMirrorHorizontalAlt2) ::FlipX(rAlt2RailSize, renderTransform);
						rOutputRect	= RIntRect(RIntSize(rAlt2RailSize)) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pHorizontalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
						if (pPath) m_pHorizontalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
						if (!m_fMirrorHorizontalAlt2) ::FlipX(rAlt2RailSize, renderTransform);
						if (!m_fInvertHorizontalAlt2) ::FlipY(rAlt2RailSize, renderTransform);
						renderSize.m_dx -= 2.0 * rAlt2RailSize.m_dx;
						renderTransform.PreTranslate( rAlt2RailSize.m_dx, 0.0 );
					}

					//	Render the last piece to ensure that there is complete coverage..
					tileDimension			= ( renderSize.m_dx - rHorizontalMiddleSize.m_dx ) / 2.0 / yNominalDimension;
					rAlt1RailSize.m_dx	= tileDimension * rHorizontalAlt1Size.m_dx;
					rAlt2RailSize.m_dx	= tileDimension * rHorizontalAlt2Size.m_dx;
					if ( fIsPrinting )
					{
						rRenderRailSize1	= rAlt1RailSize;
						rRenderRailSize2	= rAlt2RailSize;
					}
					else
					{
						rRenderRailSize1	= largeAlt1RailSize;
						rRenderRailSize2	= largeAlt2RailSize;
					}
					if (!m_fInvertHorizontalAlt1) ::FlipY(rAlt1RailSize, renderTransform);
					if (!m_fMirrorHorizontalAlt1) ::FlipX(rAlt1RailSize, renderTransform);
					rOutputRect	= RIntRect(RIntSize(rAlt2RailSize)) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pHorizontalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pHorizontalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
					if (!m_fMirrorHorizontalAlt1) ::FlipX(rAlt1RailSize, renderTransform);
					if (!m_fInvertHorizontalAlt1) ::FlipY(rAlt1RailSize, renderTransform);
					renderSize.m_dx -= 2.0 * rAlt1RailSize.m_dx;
					renderTransform.PreTranslate( rAlt1RailSize.m_dx, 0.0 );
					if (!m_fInvertHorizontalAlt2) ::FlipY(rAlt2RailSize, renderTransform);
					if (!m_fMirrorHorizontalAlt2) ::FlipX(rAlt2RailSize, renderTransform);
					rOutputRect	= RIntRect(RIntSize(rAlt2RailSize)) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pHorizontalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
					if (pPath) m_pHorizontalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
					if (!m_fMirrorHorizontalAlt2) ::FlipX(rAlt2RailSize, renderTransform);
					if (!m_fInvertHorizontalAlt2) ::FlipY(rAlt2RailSize, renderTransform);
					renderSize.m_dx -= 2.0 * rAlt2RailSize.m_dx;
					renderTransform.PreTranslate( rAlt2RailSize.m_dx, 0.0 );

					rHorizontalBoxSize	= renderSize;
					rHorizontalTransform	= renderTransform;
				}
			}  
		}
	}	//	horizontal Rails

	// Horizontal middle
	if ( m_pHorizontalMiddle )
		RenderCorners(m_pHorizontalMiddle, RRealSize(rHorizontalBoxSize.m_dx,rHorizontalMiddleSize.m_dy), FALSE, m_fInvertHorizontalMiddle, pDrawingSurface, pPath, rHorizontalBoxSize, rHorizontalTransform, rRender, rMonoColor);
}

// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::RenderVertical()
//
// Description:			Draws the corners of the border
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3BorderGraphic::RenderVertical(RDrawingSurface* pDrawingSurface, RPath* pPath, const RRealSize& rVertSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor )
{
	RIntVectorRect	rOutputRect;
	RRealSize		rRenderRailSize1;
	RRealSize		rRenderRailSize2;
	BOOLEAN			fIsPrinting	= (pDrawingSurface && pDrawingSurface->IsPrinting())? TRUE : FALSE;

	//	Calculate the inversion of the transform
	R2dTransform	invTransform( rTransform );
	invTransform.Invert( );

	// Compute graphic sizes - based on the corner size (rounded down)
	RRealSize rCornerSize(m_yBorderThickness, m_yBorderThickness);
	rCornerSize	*= rTransform;
	rCornerSize.m_dx	= floor( rCornerSize.m_dx );
	rCornerSize.m_dy	= floor( rCornerSize.m_dy );
	rCornerSize	*= invTransform;

	R2dTransform	rVerticalTransform( rTransform );
	rVerticalTransform.PreTranslate(0.0, rCornerSize.m_dy);

	RRealSize rVerticalBoxSize( rVertSize );
	RRealSize rVerticalMiddleSize(::GetYSize(m_pVerticalMiddle, m_yBorderThickness) );
	rVerticalMiddleSize			*= rTransform;
	rVerticalMiddleSize.m_dx	= floor( rVerticalMiddleSize.m_dx );
	rVerticalMiddleSize.m_dy	= floor( rVerticalMiddleSize.m_dy );
	rVerticalMiddleSize			*= invTransform;

	// Vertical connectors
	if (m_pVerticalConnector)
	{
		RRealSize rVerticalConnectorSize = ::GetYSize(m_pVerticalConnector, m_yBorderThickness);
		rVerticalConnectorSize			*= rTransform;
		rVerticalConnectorSize.m_dx	= floor( rVerticalConnectorSize.m_dx );
		rVerticalConnectorSize.m_dy	= floor( rVerticalConnectorSize.m_dy );
		rVerticalConnectorSize			*= invTransform;
		RenderCorners(m_pVerticalConnector, rVerticalConnectorSize, m_fInvertVerticalConnector, m_fMirrorVerticalConnector, pDrawingSurface, pPath, rVerticalBoxSize, rVerticalTransform, rRender, rMonoColor);
		UpdateY(rVerticalConnectorSize, rVerticalBoxSize, rVerticalTransform);
	}
		
	//	If there are some sort of rails....
	if ( m_pVerticalRail || (m_pVerticalAlt1 && m_pVerticalAlt2) )
	{
		//	Check for the special case where we are stretching the rail
		if ( m_pVerticalRail && m_fStretchVerticalRail )
		{
			// Note: This is a very rare flag.  As of 12/6/96, there is only one
			// graphic which uses it: Wooden Boards.
			//	5-8-98: There are now middle pieces in some of the NEW stretched borders...
			RRealSize rStretchedRailSize(m_yBorderThickness, rVerticalBoxSize.m_dy);
			//	If there is a middle piece, the StretchedRailSize is only 1/2 box size - middle size
			if ( m_pHorizontalMiddle )
				rStretchedRailSize.m_dy	= (rStretchedRailSize.m_dy - rVerticalMiddleSize.m_dy) / 2;

			//	Render the stretched rail
			RenderCorners(m_pVerticalRail, rStretchedRailSize, m_fInvertVerticalRail, FALSE, pDrawingSurface, pPath, rVerticalBoxSize, rVerticalTransform, rRender, rMonoColor);

			//	Adjust the box size for the middle section to render correctly
			rVerticalBoxSize.m_dy -= 2.0 * rStretchedRailSize.m_dy;
			rVerticalTransform.PreTranslate( 0.0, rStretchedRailSize.m_dy );
		}
		else
		{
			//	Here we have to perform some sort of tiling.
			sLONG			sRail;
			sLONG			sRailCount;

			//	Compute the inverse of the transform
			YRealDimension	xScale;
			YRealDimension	yScale;
			YAngle			rotation;
			rTransform.Decompose( rotation, xScale, yScale );

			//	Compute some useful values for later use
			RRealSize rVerticalRailSize = ::GetYSize(m_pVerticalRail, m_yBorderThickness);
			RRealSize rVerticalAlt1Size = ::GetYSize(m_pVerticalAlt1, m_yBorderThickness);
			RRealSize rVerticalAlt2Size = ::GetYSize(m_pVerticalAlt2, m_yBorderThickness);

			YRealDimension yVariableLength	= (rVerticalBoxSize.m_dy - rVerticalMiddleSize.m_dy) / 2.0;	
			YRealDimension	yNominalDimension	= (m_pVerticalRail)? rVerticalRailSize.m_dy : rVerticalAlt1Size.m_dy+rVerticalAlt2Size.m_dy;
			sRailCount = ::GetRailCount( yVariableLength, yNominalDimension );

			if (sRailCount > 0)
			{
				//	What is the excess
				YIntDimension	smallSize		= sRailCount;
				YIntDimension	increment		= 0;
				YRealDimension	xStartOffset	= 0.0;
				YIntDimension	excessCounter	= 0;
				R2dTransform	renderTransform;
				RRealSize		renderSize;
		
				if ( m_pVerticalRail )
				{
					RRealSize	rRailSize( rVerticalRailSize );
					// Compute the new rail sizes (the small and large ones)
					RRealSize	smallRailSize( rVerticalRailSize.m_dx, yVariableLength / (YFloatType)sRailCount );
					smallRailSize	*= rTransform;
					smallRailSize	= RRealSize( floor( smallRailSize.m_dx ), floor( smallRailSize.m_dy ) );
					RRealSize	largeRailSize( smallRailSize.m_dx, smallRailSize.m_dy+1.0 );
					//
					//	Re-expand to logical units
					smallRailSize	*= invTransform;
					largeRailSize	*= invTransform;
					//
					//	Reset the x dimension
					smallRailSize.m_dx	= largeRailSize.m_dx = rCornerSize.m_dx;
					increment				= static_cast<YIntDimension>( (yVariableLength - (smallRailSize.m_dy * sRailCount)) * yScale) + 1;

					//
					//	Render the TopLeft rails
					renderTransform	= rVerticalTransform;
					renderSize			= rVerticalBoxSize;
					excessCounter		= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Add the extra to the excessCounter
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rRailSize		= largeRailSize;
							excessCounter	-= smallSize;
						}
						else
							rRailSize	= smallRailSize;

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rRailSize;
						}
						else
						{
							rRenderRailSize1	= largeRailSize;
						}

						rOutputRect	= RIntRect( RIntSize(rRailSize) ) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pVerticalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pVerticalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
						renderSize.m_dy -= 2.0 * rRailSize.m_dy;
						renderTransform.PreTranslate( 0.0, rRailSize.m_dy );
					}

					//	Render the last piece to ensure that there is complete coverage..
					rRailSize.m_dy	= (renderSize.m_dy - rVerticalMiddleSize.m_dy) / 2.0;
					if ( fIsPrinting )
						rRenderRailSize1	= rRailSize;
					else
						rRenderRailSize1	= largeRailSize;
					rOutputRect	= RIntRect( RIntSize(rRailSize) ) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pVerticalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pVerticalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
					renderSize.m_dx -= 2.0 * rRailSize.m_dy;
					renderTransform.PreTranslate( 0.0, rRailSize.m_dy );

					//
					//	Render the Bottom Left rails
					renderTransform	= rVerticalTransform;
					::FlipY(rVerticalBoxSize, renderTransform);
					renderSize			= rVerticalBoxSize;
					excessCounter		= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Add the extra to the excessCounter
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rRailSize		= largeRailSize;
							excessCounter	-= smallSize;
						}
						else
							rRailSize	= smallRailSize;

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rRailSize;
						}
						else
						{
							rRenderRailSize1	= largeRailSize;
						}

						if (!m_fMirrorVerticalRail) ::FlipY( rRailSize, renderTransform );
						rOutputRect	= RIntRect( RIntSize(rRailSize) ) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pVerticalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pVerticalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
						if (!m_fMirrorVerticalRail) ::FlipY( rRailSize, renderTransform );
						renderSize.m_dy -= 2.0 * rRailSize.m_dy;
						renderTransform.PreTranslate( 0.0, rRailSize.m_dy );
					}

					//	Render the last piece to ensure that there is complete coverage..
					rRailSize.m_dy	= (renderSize.m_dy - rVerticalMiddleSize.m_dy) / 2.0;
					if ( fIsPrinting )
						rRenderRailSize1	= rRailSize;
					else
						rRenderRailSize1	= largeRailSize;
					if (!m_fMirrorVerticalRail) ::FlipY( rRailSize, renderTransform );
					rOutputRect	= RIntRect( RIntSize(rRailSize) ) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pVerticalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pVerticalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
					if (!m_fMirrorVerticalRail) ::FlipY( rRailSize, renderTransform );
					renderSize.m_dx -= 2.0 * rRailSize.m_dy;
					renderTransform.PreTranslate( 0.0, rRailSize.m_dy );

					//
					//	Render the Top Right rails
					renderTransform	= rVerticalTransform;
					::FlipX(rVerticalBoxSize, renderTransform);
					renderSize			= rVerticalBoxSize;
					excessCounter		= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Add the extra to the excessCounter
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rRailSize		= largeRailSize;
							excessCounter	-= smallSize;
						}
						else
							rRailSize	= smallRailSize;

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rRailSize;
						}
						else
						{
							rRenderRailSize1	= largeRailSize;
						}

						if (!m_fInvertVerticalRail) ::FlipX( rRailSize, renderTransform );
						rOutputRect	= RIntRect( RIntSize(rRailSize) ) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pVerticalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pVerticalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
						if (!m_fInvertVerticalRail) ::FlipX( rRailSize, renderTransform );
						renderSize.m_dy -= 2.0 * rRailSize.m_dy;
						renderTransform.PreTranslate( 0.0, rRailSize.m_dy );
					}

					//	Render the last piece to ensure that there is complete coverage..
					rRailSize.m_dy	= (renderSize.m_dy - rVerticalMiddleSize.m_dy) / 2.0;
					if ( fIsPrinting )
						rRenderRailSize1	= rRailSize;
					else
						rRenderRailSize1	= largeRailSize;
					if (!m_fInvertVerticalRail) ::FlipX( rRailSize, renderTransform );
					rOutputRect	= RIntRect( RIntSize(rRailSize) ) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pVerticalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pVerticalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
					if (!m_fInvertVerticalRail) ::FlipX( rRailSize, renderTransform );
					renderSize.m_dx -= 2.0 * rRailSize.m_dy;
					renderTransform.PreTranslate( 0.0, rRailSize.m_dy );

					//
					//	Render the Bottom Right rails
					renderTransform	= rVerticalTransform;
					::FlipY(rVerticalBoxSize, renderTransform);
					::FlipX(rVerticalBoxSize, renderTransform);
					renderSize			= rVerticalBoxSize;
					excessCounter		= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Add the extra to the excessCounter
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rRailSize		= largeRailSize;
							excessCounter	-= smallSize;
						}
						else
							rRailSize	= smallRailSize;

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rRailSize;
						}
						else
						{
							rRenderRailSize1	= largeRailSize;
						}

						if (!m_fMirrorVerticalRail) ::FlipY( rRailSize, renderTransform );
						if (!m_fInvertVerticalRail) ::FlipX( rRailSize, renderTransform );
						rOutputRect	= RIntRect( RIntSize(rRailSize) ) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pVerticalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pVerticalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
						if (!m_fInvertVerticalRail) ::FlipX( rRailSize, renderTransform );
						if (!m_fMirrorVerticalRail) ::FlipY( rRailSize, renderTransform );
						renderSize.m_dy -= 2.0 * rRailSize.m_dy;
						renderTransform.PreTranslate( 0.0, rRailSize.m_dy );
					}

					//	Render the last piece to ensure that there is complete coverage..
					rRailSize.m_dy	= (renderSize.m_dy - rVerticalMiddleSize.m_dy) / 2.0;
					if ( fIsPrinting )
						rRenderRailSize1	= rRailSize;
					else
						rRenderRailSize1	= largeRailSize;
					if (!m_fMirrorVerticalRail) ::FlipY( rRailSize, renderTransform );
					if (!m_fInvertVerticalRail) ::FlipX( rRailSize, renderTransform );
					rOutputRect	= RIntRect( RIntSize(rRailSize) ) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pVerticalRail->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pVerticalRail->GetOutlinePath(*pPath, rRailSize, renderTransform);
					if (!m_fInvertVerticalRail) ::FlipX( rRailSize, renderTransform );
					if (!m_fMirrorVerticalRail) ::FlipY( rRailSize, renderTransform );
					renderSize.m_dy -= 2.0 * rRailSize.m_dy;
					renderTransform.PreTranslate( 0.0, rRailSize.m_dy );

					rVerticalBoxSize		= renderSize;
					rVerticalTransform	= renderTransform;
				}
				else	//	must be if (m_pVertialAlt1 && m_pVerticalAlt2)
				{
					RRealSize	rAlt1RailSize( rVerticalAlt1Size );
					RRealSize	rAlt2RailSize( rVerticalAlt2Size );
				
					// Compute the new rail sizes (the small and large ones)
					YRealDimension	tileDimension( yVariableLength / (YFloatType)sRailCount / yNominalDimension );
					RRealSize		smallAlt1RailSize( rVerticalAlt1Size.m_dx, tileDimension * rVerticalAlt1Size.m_dy );
					RRealSize		smallAlt2RailSize( rVerticalAlt2Size.m_dx, tileDimension * rVerticalAlt2Size.m_dy );
					smallAlt1RailSize	*= rTransform;
					smallAlt2RailSize	*= rTransform;
					smallAlt1RailSize	= RRealSize( floor( smallAlt1RailSize.m_dx ), floor( smallAlt1RailSize.m_dy ) );
					smallAlt2RailSize	= RRealSize( floor( smallAlt2RailSize.m_dx ), floor( smallAlt2RailSize.m_dy ) );
					RRealSize		largeAlt1RailSize( smallAlt1RailSize.m_dx, smallAlt1RailSize.m_dy+1.0 );
					RRealSize		largeAlt2RailSize( smallAlt1RailSize.m_dx, smallAlt1RailSize.m_dy+1.0 );
					//
					//	Re-expand to logical units
					smallAlt1RailSize	*= invTransform;
					smallAlt2RailSize	*= invTransform;
					largeAlt1RailSize	*= invTransform;
					largeAlt2RailSize	*= invTransform;
					//
					//	Reset the y dimension
					smallAlt1RailSize.m_dx = largeAlt1RailSize.m_dx = rCornerSize.m_dx;
					smallAlt2RailSize.m_dx = largeAlt2RailSize.m_dx = rCornerSize.m_dx;
					increment					= static_cast<YIntDimension>( (yVariableLength - ((smallAlt1RailSize.m_dy+smallAlt2RailSize.m_dy) * sRailCount)) * yScale) + 1;

					//
					//		Render the TopLeft Vertical Tiles
					renderTransform		= rVerticalTransform;
					renderSize				= rVerticalBoxSize;
					excessCounter			= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Setup the RailSize...
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rAlt1RailSize	= largeAlt1RailSize;
							rAlt2RailSize	= largeAlt2RailSize;
							excessCounter	-= smallSize;
						}
						else
						{
							rAlt1RailSize	= smallAlt1RailSize;
							rAlt2RailSize	= smallAlt2RailSize;
						}

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rAlt1RailSize;
							rRenderRailSize2	= rAlt2RailSize;
						}
						else
						{
							rRenderRailSize1	= largeAlt1RailSize;
							rRenderRailSize2	= largeAlt2RailSize;
						}
						rOutputRect	= RIntRect( RIntSize(rAlt1RailSize) ) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pVerticalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pVerticalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
						renderSize.m_dy -= 2.0 * rAlt1RailSize.m_dy;
						renderTransform.PreTranslate( 0.0, rAlt1RailSize.m_dy );
						rOutputRect	= RIntRect( RIntSize(rAlt2RailSize) ) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pVerticalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
						if (pPath) m_pVerticalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
						renderSize.m_dy -= 2.0 * rAlt2RailSize.m_dy;
						renderTransform.PreTranslate( 0.0, rAlt2RailSize.m_dy );
					}

					//	Render the last piece to ensure that there is complete coverage..
					tileDimension			= ( renderSize.m_dy - rVerticalMiddleSize.m_dy ) / 2.0 / yNominalDimension;
					rAlt1RailSize.m_dy	= tileDimension * rVerticalAlt1Size.m_dy;
					rAlt2RailSize.m_dy	= tileDimension * rVerticalAlt2Size.m_dy;
					if ( fIsPrinting )
					{
						rRenderRailSize1	= rAlt1RailSize;
						rRenderRailSize2	= rAlt2RailSize;
					}
					else
					{
						rRenderRailSize1	= largeAlt1RailSize;
						rRenderRailSize2	= largeAlt2RailSize;
					}
					rOutputRect	= RIntRect( RIntSize(rAlt1RailSize) ) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pVerticalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pVerticalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
					renderSize.m_dy -= 2.0 * rAlt1RailSize.m_dy;
					renderTransform.PreTranslate( 0.0, rAlt1RailSize.m_dy );
					rOutputRect	= RIntRect( RIntSize(rAlt2RailSize) ) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pVerticalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
					if (pPath) m_pVerticalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
					renderSize.m_dy -= 2.0 * rAlt2RailSize.m_dy;
					renderTransform.PreTranslate( 0.0, rAlt2RailSize.m_dy );

					//
					//		Render the Bottom Left Vertical Tiles
					renderTransform		= rVerticalTransform;
					::FlipY(rVerticalBoxSize, renderTransform);
					renderSize				= rVerticalBoxSize;
					excessCounter			= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Setup the RailSize...
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rAlt1RailSize	= largeAlt1RailSize;
							rAlt2RailSize	= largeAlt2RailSize;
							excessCounter	-= smallSize;
						}
						else
						{
							rAlt1RailSize	= smallAlt1RailSize;
							rAlt2RailSize	= smallAlt2RailSize;
						}

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rAlt1RailSize;
							rRenderRailSize2	= rAlt2RailSize;
						}
						else
						{
							rRenderRailSize1	= largeAlt1RailSize;
							rRenderRailSize2	= largeAlt2RailSize;
						}

						if (!m_fMirrorVerticalAlt1) ::FlipY( rAlt1RailSize, renderTransform );
						rOutputRect	= RIntRect( RIntSize(rAlt1RailSize) ) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pVerticalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pVerticalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
						if (!m_fMirrorVerticalAlt1) ::FlipY( rAlt1RailSize, renderTransform );
						renderSize.m_dy -= 2.0 * rAlt1RailSize.m_dy;
						renderTransform.PreTranslate( 0.0, rAlt1RailSize.m_dy );
						if (!m_fMirrorVerticalAlt2) ::FlipY( rAlt2RailSize, renderTransform );
						rOutputRect	= RIntRect( RIntSize(rAlt2RailSize) ) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pVerticalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
						if (pPath) m_pVerticalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
						if (!m_fMirrorVerticalAlt2) ::FlipY( rAlt2RailSize, renderTransform );
						renderSize.m_dy -= 2.0 * rAlt2RailSize.m_dy;
						renderTransform.PreTranslate( 0.0, rAlt2RailSize.m_dy );
					}

					//	Render the last piece to ensure that there is complete coverage..
					tileDimension			= ( renderSize.m_dy - rVerticalMiddleSize.m_dy ) / 2.0 / yNominalDimension;
					rAlt1RailSize.m_dy	= tileDimension * rVerticalAlt1Size.m_dy;
					rAlt2RailSize.m_dy	= tileDimension * rVerticalAlt2Size.m_dy;
					if ( fIsPrinting )
					{
						rRenderRailSize1	= rAlt1RailSize;
						rRenderRailSize2	= rAlt2RailSize;
					}
					else
					{
						rRenderRailSize1	= largeAlt1RailSize;
						rRenderRailSize2	= largeAlt2RailSize;
					}

					if (!m_fMirrorVerticalAlt1) ::FlipY( rAlt1RailSize, renderTransform );
					rOutputRect	= RIntRect( RIntSize(rAlt1RailSize) ) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pVerticalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pVerticalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
					if (!m_fMirrorVerticalAlt1) ::FlipY( rAlt1RailSize, renderTransform );
					renderSize.m_dy -= 2.0 * rAlt1RailSize.m_dy;
					renderTransform.PreTranslate( 0.0, rAlt1RailSize.m_dy );
					if (!m_fMirrorVerticalAlt2) ::FlipY( rAlt2RailSize, renderTransform );
					rOutputRect	= RIntRect( RIntSize(rAlt2RailSize) ) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pVerticalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
					if (pPath) m_pVerticalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
					if (!m_fMirrorVerticalAlt2) ::FlipY( rAlt2RailSize, renderTransform );
					renderSize.m_dy -= 2.0 * rAlt2RailSize.m_dy;
					renderTransform.PreTranslate( 0.0, rAlt2RailSize.m_dy );

					//
					//		Render the Top Right Vertical Tiles
					renderTransform		= rVerticalTransform;
					::FlipX(rVerticalBoxSize, renderTransform);
					renderSize				= rVerticalBoxSize;
					excessCounter			= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Setup the RailSize...
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rAlt1RailSize	= largeAlt1RailSize;
							rAlt2RailSize	= largeAlt2RailSize;
							excessCounter	-= smallSize;
						}
						else
						{
							rAlt1RailSize	= smallAlt1RailSize;
							rAlt2RailSize	= smallAlt2RailSize;
						}

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rAlt1RailSize;
							rRenderRailSize2	= rAlt2RailSize;
						}
						else
						{
							rRenderRailSize1	= largeAlt1RailSize;
							rRenderRailSize2	= largeAlt2RailSize;
						}

						if (!m_fInvertVerticalAlt1) ::FlipX( rAlt1RailSize, renderTransform );
						rOutputRect	= RIntRect( RIntSize(rAlt1RailSize) ) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pVerticalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pVerticalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
						if (!m_fInvertVerticalAlt1) ::FlipX( rAlt1RailSize, renderTransform );
						renderSize.m_dy -= 2.0 * rAlt1RailSize.m_dy;
						renderTransform.PreTranslate( 0.0, rAlt1RailSize.m_dy );
						if (!m_fInvertVerticalAlt2) ::FlipX( rAlt2RailSize, renderTransform );
						rOutputRect	= RIntRect( RIntSize(rAlt2RailSize) ) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pVerticalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
						if (pPath) m_pVerticalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
						if (!m_fInvertVerticalAlt2) ::FlipX( rAlt2RailSize, renderTransform );
						renderSize.m_dy -= 2.0 * rAlt2RailSize.m_dy;
						renderTransform.PreTranslate( 0.0, rAlt2RailSize.m_dy );
					}

					//	Render the last piece to ensure that there is complete coverage..
					tileDimension			= ( renderSize.m_dy - rVerticalMiddleSize.m_dy ) / 2.0 / yNominalDimension;
					rAlt1RailSize.m_dy	= tileDimension * rVerticalAlt1Size.m_dy;
					rAlt2RailSize.m_dy	= tileDimension * rVerticalAlt2Size.m_dy;
					if ( fIsPrinting )
					{
						rRenderRailSize1	= rAlt1RailSize;
						rRenderRailSize2	= rAlt2RailSize;
					}
					else
					{
						rRenderRailSize1	= largeAlt1RailSize;
						rRenderRailSize2	= largeAlt2RailSize;
					}
					if (!m_fInvertVerticalAlt1) ::FlipX( rAlt1RailSize, renderTransform );
					rOutputRect	= RIntRect( RIntSize(rAlt1RailSize) ) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pVerticalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pVerticalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
					if (!m_fInvertVerticalAlt1) ::FlipX( rAlt1RailSize, renderTransform );
					renderSize.m_dy -= 2.0 * rAlt1RailSize.m_dy;
					renderTransform.PreTranslate( 0.0, rAlt1RailSize.m_dy );
					if (!m_fInvertVerticalAlt2) ::FlipX( rAlt2RailSize, renderTransform );
					rOutputRect	= RIntRect( RIntSize(rAlt2RailSize) ) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pVerticalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
					if (pPath) m_pVerticalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
					if (!m_fInvertVerticalAlt2) ::FlipX( rAlt2RailSize, renderTransform );
					renderSize.m_dy -= 2.0 * rAlt2RailSize.m_dy;
					renderTransform.PreTranslate( 0.0, rAlt2RailSize.m_dy );

					//
					//		Render the Bottom Right Vertical Tiles
					renderTransform		= rVerticalTransform;
					::FlipY(rVerticalBoxSize, renderTransform);
					::FlipX(rVerticalBoxSize, renderTransform);
					renderSize				= rVerticalBoxSize;
					excessCounter			= 0;
					for (sRail = 0; sRail < sRailCount-1; sRail++)
					{
						//	Setup the RailSize...
						excessCounter	+= increment;
						if ( excessCounter >= smallSize )
						{
							rAlt1RailSize	= largeAlt1RailSize;
							rAlt2RailSize	= largeAlt2RailSize;
							excessCounter	-= smallSize;
						}
						else
						{
							rAlt1RailSize	= smallAlt1RailSize;
							rAlt2RailSize	= smallAlt2RailSize;
						}

						//	Only use the large/small size if printing...
						if ( fIsPrinting )
						{
							rRenderRailSize1	= rAlt1RailSize;
							rRenderRailSize2	= rAlt2RailSize;
						}
						else
						{
							rRenderRailSize1	= largeAlt1RailSize;
							rRenderRailSize2	= largeAlt2RailSize;
						}

						if (!m_fMirrorVerticalAlt1) ::FlipY( rAlt1RailSize, renderTransform );
						if (!m_fInvertVerticalAlt1) ::FlipX( rAlt1RailSize, renderTransform );
						rOutputRect	= RIntRect( RIntSize(rAlt1RailSize) ) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pVerticalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
						if (pPath) m_pVerticalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
						if (!m_fInvertVerticalAlt1) ::FlipX( rAlt1RailSize, renderTransform );
						if (!m_fMirrorVerticalAlt1) ::FlipY( rAlt1RailSize, renderTransform );
						renderSize.m_dy -= 2.0 * rAlt1RailSize.m_dy;
						renderTransform.PreTranslate( 0.0, rAlt1RailSize.m_dy );
						if (!m_fMirrorVerticalAlt2) ::FlipY( rAlt2RailSize, renderTransform );
						if (!m_fInvertVerticalAlt2) ::FlipX( rAlt2RailSize, renderTransform );
						rOutputRect	= RIntRect( RIntSize(rAlt2RailSize) ) * renderTransform;
						if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
							if (pDrawingSurface) m_pVerticalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
						if (pPath) m_pVerticalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
						if (!m_fInvertVerticalAlt2) ::FlipX( rAlt2RailSize, renderTransform );
						if (!m_fMirrorVerticalAlt2) ::FlipY( rAlt2RailSize, renderTransform );
						renderSize.m_dy -= 2.0 * rAlt2RailSize.m_dy;
						renderTransform.PreTranslate( 0.0, rAlt2RailSize.m_dy );
					}

					//	Render the last piece to ensure that there is complete coverage..
					tileDimension			= ( renderSize.m_dy - rVerticalMiddleSize.m_dy ) / 2.0 / yNominalDimension;
					rAlt1RailSize.m_dy	= tileDimension * rVerticalAlt1Size.m_dy;
					rAlt2RailSize.m_dy	= tileDimension * rVerticalAlt2Size.m_dy;
					if ( fIsPrinting )
					{
						rRenderRailSize1	= rAlt1RailSize;
						rRenderRailSize2	= rAlt2RailSize;
					}
					else
					{
						rRenderRailSize1	= largeAlt1RailSize;
						rRenderRailSize2	= largeAlt2RailSize;
					}
					if (!m_fMirrorVerticalAlt1) ::FlipY( rAlt1RailSize, renderTransform );
					if (!m_fInvertVerticalAlt1) ::FlipX( rAlt1RailSize, renderTransform );
					rOutputRect	= RIntRect( RIntSize(rAlt1RailSize) ) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pVerticalAlt1->Render(*pDrawingSurface, rRenderRailSize1, renderTransform, rRender, rMonoColor);
					if (pPath) m_pVerticalAlt1->GetOutlinePath(*pPath, rAlt1RailSize, renderTransform);
					if (!m_fInvertVerticalAlt1) ::FlipX( rAlt1RailSize, renderTransform );
					if (!m_fMirrorVerticalAlt1) ::FlipY( rAlt1RailSize, renderTransform );
					renderSize.m_dy -= 2.0 * rAlt1RailSize.m_dy;
					renderTransform.PreTranslate( 0.0, rAlt1RailSize.m_dy );
					if (!m_fMirrorVerticalAlt2) ::FlipY( rAlt2RailSize, renderTransform );
					if (!m_fInvertVerticalAlt2) ::FlipX( rAlt2RailSize, renderTransform );
					rOutputRect	= RIntRect( RIntSize(rAlt2RailSize) ) * renderTransform;
					if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
						if (pDrawingSurface) m_pVerticalAlt2->Render(*pDrawingSurface, rRenderRailSize2, renderTransform, rRender, rMonoColor);
					if (pPath) m_pVerticalAlt2->GetOutlinePath(*pPath, rAlt2RailSize, renderTransform);
					if (!m_fInvertVerticalAlt2) ::FlipX( rAlt2RailSize, renderTransform );
					if (!m_fMirrorVerticalAlt2) ::FlipY( rAlt2RailSize, renderTransform );
					renderSize.m_dy -= 2.0 * rAlt2RailSize.m_dy;
					renderTransform.PreTranslate( 0.0, rAlt2RailSize.m_dy );

					rVerticalBoxSize		= renderSize;
					rVerticalTransform	= renderTransform;
				}	
			}  
		}
	}	//	Vertical Rails

	// Vertical middle
	if ( m_pVerticalMiddle )
		RenderCorners(m_pVerticalMiddle, RRealSize(rVerticalMiddleSize.m_dx,rVerticalBoxSize.m_dy), m_fInvertVerticalMiddle, FALSE, pDrawingSurface, pPath, rVerticalBoxSize, rVerticalTransform, rRender, rMonoColor);
}

// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::RenderCorners()
//
// Description:			Draws the corners of the border
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3BorderGraphic::RenderCorners(RGraphic* pGraphic, const RRealSize& rGraphicSize, BOOLEAN fMirror, BOOLEAN fInvert, RDrawingSurface* pDrawingSurface, RPath* pPath, const RRealSize& rBorderSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor)
{
	RIntVectorRect	rOutputRect;
	// Make sure that the graphics will fit in the box
	const YFloatType	kCanFitDelta	= 0.26;	//	A little over 1/4
	YFloatType yHorizontalRatio		= ::Round( rBorderSize.m_dx / rGraphicSize.m_dx, 2);
	YFloatType yVerticalRatio			= ::Round( rBorderSize.m_dy / rGraphicSize.m_dy, 2);

	R2dTransform rGraphicTransform;

	if ( ::AreFloatsGTE( yHorizontalRatio, 1.0, kCanFitDelta ) )
	{
		// We can fit at least one graphic in the box horizontally
		// Upper left
		rGraphicTransform = rTransform;
		rOutputRect	= RIntRect( RIntSize(rGraphicSize) ) * rGraphicTransform;
		if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
			if (pDrawingSurface) pGraphic->Render(*pDrawingSurface, rGraphicSize, rGraphicTransform, rRender, rMonoColor);
		if (pPath) pGraphic->GetOutlinePath(*pPath, rGraphicSize, rGraphicTransform);

		if ( ::AreFloatsGTE( yHorizontalRatio, 2.0, kCanFitDelta ) )
		{
			// We can fit at least 2 graphics in the box horizonatally
			// Upper right
			rGraphicTransform = rTransform;
			::FlipX(rBorderSize, rGraphicTransform);
			if (!fMirror) ::FlipX(rGraphicSize, rGraphicTransform);
			rOutputRect	= RIntRect( RIntSize(rGraphicSize) ) * rGraphicTransform;
			if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
				if (pDrawingSurface) pGraphic->Render(*pDrawingSurface, rGraphicSize, rGraphicTransform, rRender, rMonoColor);
			if (pPath) pGraphic->GetOutlinePath(*pPath, rGraphicSize, rGraphicTransform);
		}
	}

	if ( ::AreFloatsGTE( yVerticalRatio, 1.0), kCanFitDelta )
	{
		// We can fit at least one graphic in the box vertically
		// Lower left
		rGraphicTransform = rTransform;
		::FlipY(rBorderSize, rGraphicTransform);
		if (!fInvert) ::FlipY(rGraphicSize, rGraphicTransform);
		rOutputRect	= RIntRect( RIntSize(rGraphicSize) ) * rGraphicTransform;
		if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
			if (pDrawingSurface) pGraphic->Render(*pDrawingSurface, rGraphicSize, rGraphicTransform, rRender, rMonoColor);
		if (pPath) pGraphic->GetOutlinePath(*pPath, rGraphicSize, rGraphicTransform);

		if ( ::AreFloatsGTE( yHorizontalRatio, 2.0, kCanFitDelta) )
		{
			// We can fit at least 2 graphics in the box vertically
			// Lower right
			rGraphicTransform = rTransform;
			::FlipX(rBorderSize, rGraphicTransform);
			::FlipY(rBorderSize, rGraphicTransform);
			if (!fMirror) ::FlipX(rGraphicSize, rGraphicTransform);
			if (!fInvert) ::FlipY(rGraphicSize, rGraphicTransform);
			rOutputRect	= RIntRect( RIntSize(rGraphicSize) ) * rGraphicTransform;
			if ( rOutputRect.m_TransformedBoundingRect.IsIntersecting( rRender ) )
				if (pDrawingSurface) pGraphic->Render(*pDrawingSurface, rGraphicSize, rGraphicTransform, rRender, rMonoColor);
			if (pPath) pGraphic->GetOutlinePath(*pPath, rGraphicSize, rGraphicTransform);
		}
	}
}

// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::GetOutlinePath()
//
// Description:			
//
// Returns:					
//
// Exceptions:				
//
// ****************************************************************************
BOOLEAN RPsd3BorderGraphic::GetOutlinePath(RPath& rPath, const RRealSize& rSize, const R2dTransform& rTransform)
{
	//	make sure rectangle is large enough to cause area to be in the outline path
	RIntRect	largeRect(-16000, -16000, 16000, 16000);
	RColor	color(kBlack);
	Render(NULL, &rPath, rSize, rTransform, largeRect, color);
	
	return TRUE;
}

// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::SetThickness()
//
// Description:			Sets m_yBorderThickness;
//
// Returns:					The thickness of the border
//	
// Exceptions:				None
//
// ****************************************************************************
void RPsd3BorderGraphic::SetBorderThickness(YRealDimension yThickness)
{
	m_yBorderThickness = yThickness;
}

// ****************************************************************************
//
// Function Name:			RPsd3BorderGraphic::GetBorderThickness()
//
// Description:			Returns the thickness of the border in RLUs
//
// Returns:					The thickness of the border
//	
// Exceptions:				None
//
// ****************************************************************************
YRealDimension RPsd3BorderGraphic::GetBorderThickness() const
{
	return m_yBorderThickness;
}

// ****************************************************************************
//
// Function Name:			Undefine()
//
// Description:			Clean up internals
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3BorderGraphic::Undefine()
{	
	delete m_pCorner;	
	delete m_pHorizontalConnector;
	delete m_pHorizontalMiddle;
	delete m_pHorizontalAlt1;
	delete m_pHorizontalAlt2;
	delete m_pVerticalConnector;
	delete m_pVerticalMiddle;
	delete m_pVerticalAlt1;
	delete m_pVerticalAlt2;

	// If we use the same piece for the entire border, we don't want to delete the corner twice
	if (m_pHorizontalRail != m_pCorner) delete m_pHorizontalRail;
	if (m_pVerticalRail != m_pCorner) delete m_pVerticalRail;

	// Set all the pointers to NULL
	m_pCorner =								NULL;
	m_pHorizontalConnector =			NULL;
	m_pHorizontalMiddle =				NULL;
	m_pHorizontalRail =					NULL;
	m_pHorizontalAlt1 =					NULL;
	m_pHorizontalAlt2 =					NULL;
	m_pVerticalConnector =				NULL;
	m_pVerticalMiddle =					NULL;
	m_pVerticalRail =						NULL;
	m_pVerticalAlt1 =						NULL;
	m_pVerticalAlt2 =						NULL;
	
	// Reset the border thickness to the default
	m_yBorderThickness = kDefaultBorderThickness;

	// Set all the flags to FALSE
	m_fMirrorCorner =						FALSE;
	m_fMirrorHorizontalConnector =	FALSE;
	m_fMirrorHorizontalRail =			FALSE;
	m_fMirrorHorizontalAlt1 =			FALSE;
	m_fMirrorHorizontalAlt2 =			FALSE;
	m_fMirrorVerticalConnector =		FALSE;
	m_fMirrorVerticalRail =				FALSE;
	m_fMirrorVerticalAlt1 =				FALSE;
	m_fMirrorVerticalAlt2 =				FALSE;
	
	m_fInvertCorner =						FALSE;
	m_fInvertHorizontalConnector =	FALSE;
	m_fInvertHorizontalRail =			FALSE;
	m_fInvertHorizontalAlt1 =			FALSE;
	m_fInvertHorizontalAlt2 =			FALSE;
	m_fInvertVerticalConnector =		FALSE;
	m_fInvertVerticalRail =				FALSE;
	m_fInvertVerticalAlt1 =				FALSE;
	m_fInvertVerticalAlt2 =				FALSE;
	m_fInvertHorizontalMiddle =		FALSE;
	m_fInvertVerticalMiddle =			FALSE;

	m_fStretchHorizontalRail =			FALSE;
	m_fStretchVerticalRail =			FALSE;
}

// ****************************************************************************
//
// Function Name:			UpdateX()
//
// Description:			
//
// Returns:					
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3BorderGraphic::UpdateX(const RRealSize& rGraphicSize, RRealSize& rBoxSize, R2dTransform& rTransform)
{
	rBoxSize.m_dx -= 2.0 * rGraphicSize.m_dx;
	rTransform.PreTranslate(rGraphicSize.m_dx, 0.0);
}

// ****************************************************************************
//
// Function Name:			UpdateY()
//
// Description:			
//
// Returns:					
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3BorderGraphic::UpdateY(const RRealSize& rGraphicSize, RRealSize& rBoxSize, R2dTransform& rTransform)
{
	rBoxSize.m_dy -= 2.0 * rGraphicSize.m_dy;
	rTransform.PreTranslate(0.0, rGraphicSize.m_dy);
}

// ****************************************************************************
//
//  Function Name:	RPsd3BorderGraphic::OnXDisplayChange( )
//
//  Description:		Called when the display resolution changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPsd3BorderGraphic::OnXDisplayChange( )
	{
	CallDisplayChange(m_pCorner);
	CallDisplayChange(m_pHorizontalConnector);
	CallDisplayChange(m_pHorizontalMiddle);
	CallDisplayChange(m_pHorizontalRail);
	CallDisplayChange(m_pHorizontalAlt1);
	CallDisplayChange(m_pHorizontalAlt2);
	CallDisplayChange(m_pVerticalConnector);
	CallDisplayChange(m_pVerticalMiddle);
	CallDisplayChange(m_pVerticalRail);
	CallDisplayChange(m_pVerticalAlt1);
	CallDisplayChange(m_pVerticalAlt2);
	}

#ifdef TPSDEBUG
// Debugging support
void RPsd3BorderGraphic::Validate() const
{
}
#endif // TPSDEBUG

