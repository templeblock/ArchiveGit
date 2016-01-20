// *****************************************************************************
//
// File name:			Psd3BannerGraphic.cpp
//	
// Project:				Renaissance Framework
//
// Author:				G. Brown
//
// Description:		Definition of the RPsd3HorizontalBannerGraphic and 
//							RPsd3VerticalBannerGraphic classes.
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

#include "Psd3BannerGraphic.h"
#include "RailGraphicUtility.h"

// A mutli-part graphic header preceeds the actual graphic data in all 
// types of multi-part graphics.  Although the structure of this header
// is constant for all multi-part graphics, valid entries in the structure
// vary by graphic type.  The following documentation lists the valid
// combinations of flags and graphics for the RPsd3RuledLineGraphic class,
// defined by EBannerBackdropElement in RHorizontalBannerBackdrop and 
// EBannerElement in RVerticalBannerBackdrop.  For the following description,
// these relationships can be assumed,
//
//		kLeftCap = kTopCap
//		kRightCap = kBottomCap
//		kTopRail = kLeftRail
//		kBottomRail = kRightRail;
//
//	since the only significant difference between horizontal and vertical banners
// is the orientation of the graphics.
//
//	RPsd3MultiPartGraphicHeader members:
//	uWORD m_PartsList
//		kLeftCap (kTopCap) required
//		other elements are optional
//
// uWORD m_Draw;
//		Not used by Renaissance.
//
// uWORD m_Flip;
//		Indicates that a mirror image of the graphic is to be displayed instead of the original
//		graphic on opposite ends of the line.  The only valid entry in this field is 
//		kLeftCap (kTopCap), which muct be set if kRightCap (kBottomCap) is not set in m_PartsList.
//
// uWORD m_Stretch;	
//		Not valid for banner graphics.
//
// uWORD m_Unused[24];
//		Not used by Renaissance.
//
// The graphics are stored in the following order (provided they exist):
//
//		Left cap (Top cap)
//		Right cap (Bottom cap)
//		Top rail (left rail)
//		Bottom rail (right rail)


// ****************************************************************************
//
// Function Name:			RPsd3HorizontalBannerGraphic::RPsd3HorizontalBannerGraphic()
//
// Description:			Constructor
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
RPsd3HorizontalBannerGraphic::RPsd3HorizontalBannerGraphic()
:	m_pLeftCap(NULL),
	m_pRightCap(NULL),
	m_pTopRail(NULL),
	m_pBottomRail(NULL)
{
}

// ****************************************************************************
//
// Function Name:			RPsd3HorizontalBannerGraphic::~RPsd3HorizontalBannerGraphic()
//
// Description:			Destructor
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
RPsd3HorizontalBannerGraphic::~RPsd3HorizontalBannerGraphic()
{
	delete m_pLeftCap;
	delete m_pRightCap;
	delete m_pTopRail;	
	delete m_pBottomRail;
}

// ****************************************************************************
//
// Function Name:			RPsd3HorizontalBannerGraphic::Initialize()
//
// Description:			Loads a graphic into memory based on parameters
//
// Returns:					TRUE if successful, FALSE otherwise
//
// Exceptions:				None
//
// ****************************************************************************
BOOLEAN RPsd3HorizontalBannerGraphic::Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth, BOOLEAN fMonochrome)
{		
	// Clear out any previous internal data
	Undefine();

	try
	{
		// Get the header and graphic offset
		RPsd3MultiPartGraphicHeader* pHeader = (RPsd3MultiPartGraphicHeader*) pGraphicData;
		sLONG sGraphicOffset = sizeof(RPsd3MultiPartGraphicHeader);

		// byte swap data on mac
#ifdef MAC
		tintSwapStructs( pHeader, sizeof(RPsd3MultiPartGraphicHeader), 1, 0x0000000155L ); 
#endif

		// Read in the left cap
		if (pHeader->m_PartsList & kLeftCap)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;				
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome)) throw;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the graphic
			m_pLeftCap = pGraphic;
		}

		// Read in the right cap
		if (pHeader->m_PartsList & kRightCap)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;				
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome)) throw;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the graphic
			m_pRightCap = pGraphic;
		}

		// Read in the top rail
		if (pHeader->m_PartsList & kTopRail)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;				
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome)) throw;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the graphic
			m_pTopRail = pGraphic;
		}
	
		// Read in the bottom rail
		if (pHeader->m_PartsList & kBottomRail)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;				
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome)) throw;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the graphic
			m_pBottomRail = pGraphic;
		}

		// Check for mirror left cap
		m_fMirrorCaps = FALSE;
		if (pHeader->m_Flip & kLeftCap)
		{
			m_fMirrorCaps = TRUE;
			if (pHeader->m_PartsList & kRightCap)
			{
				// The header said to mirror the caps, but we were given a right
				// cap, so use it
				m_fMirrorCaps = FALSE;
			}
		}
	}
	catch(...)
	{
		// Clean up
		Undefine();
		return FALSE;
	}
	
	return TRUE;
}

// ****************************************************************************
//
// Function Name:			RPsd3HorizontalBannerGraphic::ContainsGradientData()
//
// Description:			Returns TRUE if this graphic contains gradient data
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
BOOLEAN RPsd3HorizontalBannerGraphic::ContainsGradientData()
{
	if (m_pLeftCap && m_pLeftCap->ContainsGradientData()) return TRUE;	
	else if (m_pRightCap && m_pRightCap->ContainsGradientData()) return TRUE;	
	else if (m_pTopRail && m_pTopRail->ContainsGradientData()) return TRUE;
	else if (m_pBottomRail && m_pBottomRail->ContainsGradientData()) return TRUE;
	else return FALSE;
}

// ****************************************************************************
//
// Function Name:			RPsd3HorizontalBannerGraphic::Render()
//
// Description:			Draws the Backdrop
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3HorizontalBannerGraphic::Render(RDrawingSurface& rDrawingSurface, const RRealSize& rSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor, BOOLEAN)
{
	// Compute the sizes of the elements
	RRealSize rLeftCapSize = ::GetXSize(m_pLeftCap, rSize.m_dy);
	RRealSize rRightCapSize = (m_fMirrorCaps) ? ::GetXSize(m_pLeftCap, rSize.m_dy) : ::GetXSize(m_pRightCap, rSize.m_dy);
	RRealSize rTopRailSize = ::GetXSize(m_pTopRail, rSize.m_dy / 2.0);
	RRealSize rBottomRailSize = ::GetXSize(m_pBottomRail, rSize.m_dy / 2.0);

	R2dTransform rGraphicTransform;	
	
	// Left cap
	rGraphicTransform = rTransform;
	if (m_pLeftCap) m_pLeftCap->Render(rDrawingSurface, rLeftCapSize, rGraphicTransform, rRender, rMonoColor);

	
	// Right cap
	rGraphicTransform = rTransform;
	::FlipX(rSize, rGraphicTransform);
	if (m_fMirrorCaps)
	{
		// Render the mirrored left cap as the right cap
		if (m_pLeftCap) m_pLeftCap->Render(rDrawingSurface, rLeftCapSize, rGraphicTransform, rRender, rMonoColor);
	}
	else
	{
		// Render the right cap
		::FlipX(rRightCapSize, rGraphicTransform);
		if (m_pRightCap) m_pRightCap->Render(rDrawingSurface, rRightCapSize, rGraphicTransform, rRender, rMonoColor);
	}

	
	// Rails
	YRealDimension yVariableLength = rSize.m_dx - rLeftCapSize.m_dx - rRightCapSize.m_dx;
	uLONG uRailCount = 0;
	uLONG uRail = 0;

	// Top rails
	uRailCount = ::GetRailCount(yVariableLength, rTopRailSize.m_dx);
	if (uRailCount > 0)
	{
		// Adjust the top rail size
		rTopRailSize.m_dx = yVariableLength / (YFloatType)uRailCount;
		
		rGraphicTransform = rTransform;
		rGraphicTransform.PreTranslate(rLeftCapSize.m_dx, 0.0);
		for (uRail = 0; uRail < uRailCount; uRail++)
		{
			if (m_pTopRail) m_pTopRail->Render(rDrawingSurface, rTopRailSize, rGraphicTransform, rRender, rMonoColor);
			rGraphicTransform.PreTranslate(rTopRailSize.m_dx, 0.0);
		}
	}

	// Bottom rails	
	uRailCount = ::GetRailCount(yVariableLength, rBottomRailSize.m_dx);
	if (uRailCount > 0)
	{
		// Adjust the bottom rail size
		rBottomRailSize.m_dx = yVariableLength / (YFloatType)uRailCount;

		rGraphicTransform = rTransform;
		rGraphicTransform.PreTranslate(rLeftCapSize.m_dx, rSize.m_dy / 2.0);
		for (uRail = 0; uRail < uRailCount; uRail++)
		{
			if (m_pBottomRail) m_pBottomRail->Render(rDrawingSurface, rBottomRailSize, rGraphicTransform, rRender, rMonoColor);
			rGraphicTransform.PreTranslate(rBottomRailSize.m_dx, 0.0);
		}
	}
}

// ****************************************************************************
//
// Function Name:		RPsd3HorizontalBannerGraphic::GetOutlinePath( )
//
// Description:		Compute and return the path the encompases the path of this graphic
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
BOOLEAN RPsd3HorizontalBannerGraphic::GetOutlinePath( RPath&, const RRealSize&, const R2dTransform&)
{
	// REVIEW
	return FALSE;
}

// ****************************************************************************
//
// Function Name:			RPsd3HorizontalBannerGraphic::Undefine()
//
// Description:			
//
// Returns:					
//	
// Exceptions:				
//
// ****************************************************************************
void RPsd3HorizontalBannerGraphic::Undefine()
{
	delete m_pLeftCap;
	m_pLeftCap = NULL;
	delete m_pRightCap;
	m_pRightCap = NULL;
	delete m_pTopRail;
	m_pTopRail = NULL;
	delete m_pBottomRail;
	m_pBottomRail = NULL;
}

// ****************************************************************************
//
// Function Name:			RPsd3VerticalBannerGraphic::RPsd3VerticalBannerGraphic()
//
// Description:			Constructor
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
RPsd3VerticalBannerGraphic::RPsd3VerticalBannerGraphic()
:	m_pTopCap(NULL),
	m_pBottomCap(NULL),
	m_pLeftRail(NULL),
	m_pRightRail(NULL)
{
}

// ****************************************************************************
//
// Function Name:			RPsd3VerticalBannerGraphic::~RPsd3VerticalBannerGraphic()
//
// Description:			Destructor
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
RPsd3VerticalBannerGraphic::~RPsd3VerticalBannerGraphic()
{
	delete m_pTopCap;
	delete m_pBottomCap;
	delete m_pLeftRail;	
	delete m_pRightRail;
}

// ****************************************************************************
//
// Function Name:			RPsd3VerticalBannerGraphic::Initialize()
//
// Description:			Loads a graphic into memory based on parameters
//
// Returns:					TRUE if successful, FALSE otherwise
//
// Exceptions:				None
//
// ****************************************************************************
BOOLEAN RPsd3VerticalBannerGraphic::Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth, BOOLEAN fMonochrome)
{		
	// Clear out any previous internal data
	Undefine();

	try
	{
		// Get the header and graphic offset
		RPsd3MultiPartGraphicHeader* pHeader = (RPsd3MultiPartGraphicHeader*) pGraphicData;
		sLONG sGraphicOffset = sizeof(RPsd3MultiPartGraphicHeader);

		// byte swap data on mac
#ifdef MAC
		tintSwapStructs( pHeader, sizeof(RPsd3MultiPartGraphicHeader), 1, 0x0000000155L ); 
#endif

		// Read in the left cap
		if (pHeader->m_PartsList & kTopCap)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;				
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome)) throw;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the graphic
			m_pTopCap = pGraphic;
		}

		// Read in the right cap
		if (pHeader->m_PartsList & kBottomCap)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;				
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome)) throw;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the graphic
			m_pBottomCap = pGraphic;
		}

		// Read in the top rail
		if (pHeader->m_PartsList & kLeftRail)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;				
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome)) throw;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the graphic
			m_pLeftRail = pGraphic;
		}
	
		// Read in the bottom rail
		if (pHeader->m_PartsList & kRightRail)
		{
			const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;				
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome)) throw;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the graphic
			m_pRightRail = pGraphic;
		}

		// Check for mirror left cap
		m_fMirrorCaps = FALSE;
		if (pHeader->m_Flip & kTopCap)
		{
			m_fMirrorCaps = TRUE;
			if (pHeader->m_PartsList & kBottomCap)
			{
				// The header said to mirror the caps, but we were given a right
				// cap, so use it
				m_fMirrorCaps = FALSE;
			}
		}
	}
	catch(...)
	{
		// Clean up
		Undefine();
		return FALSE;
	}
	
	return TRUE;
}

// ****************************************************************************
//
// Function Name:			RPsd3VerticalBannerGraphic::ContainsGradientData()
//
// Description:			Returns TRUE if this graphic contains gradient data
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
BOOLEAN RPsd3VerticalBannerGraphic::ContainsGradientData()
{
	if (m_pTopCap && m_pTopCap->ContainsGradientData()) return TRUE;	
	else if (m_pBottomCap && m_pBottomCap->ContainsGradientData()) return TRUE;	
	else if (m_pLeftRail && m_pLeftRail->ContainsGradientData()) return TRUE;
	else if (m_pRightRail && m_pRightRail->ContainsGradientData()) return TRUE;
	else return FALSE;
}

// ****************************************************************************
//
// Function Name:			RPsd3VerticalBannerGraphic::Render()
//
// Description:			Draws the Backdrop
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3VerticalBannerGraphic::Render(RDrawingSurface& rDrawingSurface, const RRealSize& rSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor, BOOLEAN)
{
	// Compute the sizes of the elements
	RRealSize rTopCapSize = ::GetYSize(m_pTopCap, rSize.m_dx);
	RRealSize rBottomCapSize = (m_fMirrorCaps) ? ::GetYSize(m_pTopCap, rSize.m_dx) : ::GetXSize(m_pBottomCap, rSize.m_dx);
	RRealSize rLeftRailSize = ::GetYSize(m_pLeftRail, rSize.m_dx / 2.0);
	RRealSize rRightRailSize = ::GetYSize(m_pRightRail, rSize.m_dx / 2.0);

	R2dTransform rGraphicTransform;	
	
	// Left cap
	rGraphicTransform = rTransform;
	if (m_pTopCap) m_pTopCap->Render(rDrawingSurface, rTopCapSize, rGraphicTransform, rRender, rMonoColor);

	
	// Right cap
	rGraphicTransform = rTransform;
	::FlipY(rSize, rGraphicTransform);
	if (m_fMirrorCaps)
	{
		// Render the mirrored left cap as the right cap
		if (m_pTopCap) m_pTopCap->Render(rDrawingSurface, rTopCapSize, rGraphicTransform, rRender, rMonoColor);
	}
	else
	{
		// Render the right cap
		::FlipY(rBottomCapSize, rGraphicTransform);
		if (m_pBottomCap) m_pBottomCap->Render(rDrawingSurface, rBottomCapSize, rGraphicTransform, rRender, rMonoColor);
	}

	
	// Rails
	YRealDimension yVariableLength = rSize.m_dy - rTopCapSize.m_dy - rBottomCapSize.m_dy;
	uLONG uRailCount = 0;
	uLONG uRail = 0;

	// Top rails
	uRailCount = ::GetRailCount(yVariableLength, rLeftRailSize.m_dy);
	if (uRailCount > 0)
	{
		// Adjust the top rail size
		rLeftRailSize.m_dy = yVariableLength / (YFloatType)uRailCount;
		
		rGraphicTransform = rTransform;
		rGraphicTransform.PreTranslate(0.0, rTopCapSize.m_dy);
		for (uRail = 0; uRail < uRailCount; uRail++)
		{
			if (m_pLeftRail) m_pLeftRail->Render(rDrawingSurface, rLeftRailSize, rGraphicTransform, rRender, rMonoColor);
			rGraphicTransform.PreTranslate(0.0, rLeftRailSize.m_dy);
		}
	}

	// Bottom rails	
	uRailCount = ::GetRailCount(yVariableLength, rRightRailSize.m_dy);
	if (uRailCount > 0)
	{
		// Adjust the bottom rail size
		rRightRailSize.m_dy = yVariableLength / (YFloatType)uRailCount;

		rGraphicTransform = rTransform;
		rGraphicTransform.PreTranslate(rSize.m_dx / 2.0, rTopCapSize.m_dy);
		for (uRail = 0; uRail < uRailCount; uRail++)
		{
			if (m_pRightRail) m_pRightRail->Render(rDrawingSurface, rRightRailSize, rGraphicTransform, rRender, rMonoColor);
			rGraphicTransform.PreTranslate(0.0, rRightRailSize.m_dy);
		}
	}
}

// ****************************************************************************
//
// Function Name:		RPsd3VerticalBannerGraphic::GetOutlinePath( )
//
// Description:		Compute and return the path the encompases the path of this graphic
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
BOOLEAN RPsd3VerticalBannerGraphic::GetOutlinePath( RPath&, const RRealSize&, const R2dTransform&)
{
	// REVIEW
	return FALSE;
}

// ****************************************************************************
//
// Function Name:			RPsd3VerticalBannerGraphic::Undefine()
//
// Description:			
//
// Returns:					
//	
// Exceptions:				
//
// ****************************************************************************
void RPsd3VerticalBannerGraphic::Undefine()
{
	delete m_pTopCap;
	m_pTopCap = NULL;
	delete m_pBottomCap;
	m_pBottomCap = NULL;
	delete m_pLeftRail;
	m_pLeftRail = NULL;
	delete m_pRightRail;
	m_pRightRail = NULL;
}
