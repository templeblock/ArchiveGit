// *****************************************************************************
//
// File name:		Psd3RuledLineGraphic.cpp
//
// Project:			Renaissance Framework
//
// Author:			G. Brown
//
// Description:		Definition of the RPsd3RuledLineGraphic class.
//
// Portability:		Platform-independent
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, MA 02158
//						(617) 332-0202
//
// Client:			Broderbund Software, Inc.
//
// Copyright (c) 1995-1996 Turning Point Software.  All rights reserved.
//
// *****************************************************************************

#include	"GraphicIncludes.h"

ASSERTNAME

#include "Psd3RuledLineGraphic.h"
#include "RailGraphicUtility.h"

// A mutli-part graphic header preceeds the actual graphic data in all 
// types of multi-part graphics.  Although the structure of this header
// is constant for all multi-part graphics, valid entries in the structure
// vary by graphic type.  The following documentation lists the valid
// combinations of flags and graphics for the RPsd3RuledLineGraphic class,
// defined by EPsd3RuledLineElement.

//	RPsd3MultiPartGraphicHeader members:
//	uWORD m_PartsList
//		Indicates existence of a particular piece of a multi-part graphic.
//		kPsd3RuledLineLeftCap is required.  Combinations of other elements may include:
//
//		kPsd3RuledLineRightCap
//		kPsd3RuledLineRail
//
// uWORD m_Draw;
//		Not used by Renaissance.
//
// uWORD m_Flip;
//		Indicates that a mirror image of the graphic is to be displayed instead of the original
//		graphic on opposite ends of the line.  The only valid entry in this field is 
//		kPsd3RuledLineLeftCap, which muct be set if kPsd3RuledLineRightCap is not set in m_PartsList.
//
// uWORD m_Stretch;	
//		Not valid for ruled line graphics.
//
// uWORD m_Unused[24];
//		Not used by Renaissance.
//
// The graphics are stored in the following order (provided they exist):
//
//		Left cap
//		Right cap
//		Rail


// ****************************************************************************
//
// Function Name:			RPsd3RuledLineGraphic::RPsd3RuledLineGraphic()
//
// Description:			Constructor
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
RPsd3RuledLineGraphic::RPsd3RuledLineGraphic()
:	kPsd3RuledLineLeftCap(1), 
	kPsd3RuledLineRightCap(2),
	kPsd3RuledLineRail(4),
	m_pLeftCap(NULL),
	m_pRightCap(NULL),
	m_pRail(NULL),
	m_fMirrorCaps(FALSE)
{
	SetHorizontal();
}

// ****************************************************************************
//
// Function Name:			RPsd3RuledLineGraphic::~RPsd3RuledLineGraphic()
//
// Description:			Destructor
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
RPsd3RuledLineGraphic::~RPsd3RuledLineGraphic()
{
	delete m_pLeftCap;
	delete m_pRightCap;
	delete m_pRail;	
}

// ****************************************************************************
//
// Function Name:			RPsd3RuledLineGraphic::Initialize()
//
// Description:			Loads a graphic into memory based on parameters
//
// Returns:					TRUE if successful, FALSE otherwise
//
// Exceptions:				None
//
// ****************************************************************************
BOOLEAN RPsd3RuledLineGraphic::Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth, BOOLEAN fMonochrome)
{
	// Clean up the internals
	Undefine();

	// Read in parts from file data	
	//		a) Left cap
	//		b) Right cap
	//		c) Rail	
	RPsd3MultiPartGraphicHeader* pHeader = (RPsd3MultiPartGraphicHeader*) pGraphicData;
	sLONG sGraphicOffset = sizeof(RPsd3MultiPartGraphicHeader);

	// byte swap data on mac
#ifdef MAC
	tintSwapStructs( pHeader, sizeof(RPsd3MultiPartGraphicHeader), 1, 0x0000000155L ); 
#endif

	if (pHeader->m_PartsList & kPsd3RuledLineLeftCap)
	{
		const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
		RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
		if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
		{
			delete pGraphic;			
			return FALSE;
		}
		else
		{
			m_pLeftCap = pGraphic;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
		}
	}
	
	if (pHeader->m_PartsList & kPsd3RuledLineRightCap)
	{
		const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
		RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
		if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
		{
			delete pGraphic;			
			return FALSE;
		}
		else
		{
			m_pRightCap = pGraphic;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
		}
	}

	if (pHeader->m_PartsList & kPsd3RuledLineRail)
	{
		const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
		RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;
		if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome))
		{
			delete pGraphic;			
			return FALSE;
		}
		else
		{
			m_pRail = pGraphic;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the current graphic
		}
	}

	// Check for mirror left cap
	if (pHeader->m_Flip & kPsd3RuledLineLeftCap)
	{
		m_fMirrorCaps = TRUE;
	}
	else
	{
		m_fMirrorCaps = FALSE;
	}

	return TRUE;
}

// ****************************************************************************
//
// Function Name:			RPsd3RuledLineGraphic::ContainsGradientData()
//
// Description:			Returns TRUE if this graphic contains gradient data
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
BOOLEAN RPsd3RuledLineGraphic::ContainsGradientData()
{
	if (m_pLeftCap && m_pLeftCap->ContainsGradientData()) return TRUE;
	else if (m_pRightCap && m_pRightCap->ContainsGradientData()) return TRUE;
	else if (m_pRail && m_pRail->ContainsGradientData()) return TRUE;
	else return TRUE;
}

// ****************************************************************************
//
// Function Name:			RPsd3RuledLineGraphic::Render()
//
// Description:			Draws the RuledLine
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3RuledLineGraphic::Render(RDrawingSurface& rDrawingSurface, const RRealSize& rSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor, BOOLEAN)
{
	R2dTransform rLineTransform = rTransform;
	RRealSize rLineSize = rSize;
	if (!m_fHorizontal)
	{
		rLineSize.m_dx = rSize.m_dy;
		rLineSize.m_dy = rSize.m_dx;
		rLineTransform.PreRotateAboutOrigin(::DegreesToRadians(-90.0));
		rLineTransform.PreTranslate(0.0, -rLineSize.m_dy);		
	}

	Render(&rDrawingSurface, NULL, rLineSize, rLineTransform, rRender, rMonoColor);
}

// ****************************************************************************
//
// Function Name:			RPsd3RuledLineGraphic::Render()
//
// Description:			Draws the RuledLine
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3RuledLineGraphic::Render(RDrawingSurface* pDS, RPath* pPath, const RRealSize& rSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor)
{
	// Left cap
	RenderLeftCap(pDS, pPath, rSize, rTransform, rRender, rMonoColor);
	
	// Right cap
	RenderRightCap(pDS, pPath, rSize, rTransform, rRender, rMonoColor);

	// Rails
	RenderRails(pDS, pPath, rSize, rTransform, rRender, rMonoColor);
}

// ****************************************************************************
//
// Function Name:			RPsd3RuledLineGraphic::RenderLeftCap()
//
// Description:			Draws the left cap of the Ruled Line
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3RuledLineGraphic::RenderLeftCap(RDrawingSurface* pDS, RPath* pPath, const RRealSize& rLineSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor) const
{
	if (!m_pLeftCap) return;

	RRealSize rLeftCapSize;
	rLeftCapSize.m_dy = rLineSize.m_dy;
	rLeftCapSize.m_dx = rLineSize.m_dy * m_pLeftCap->GetAspectRatio();

	//	If the line is real small AND the line is not that large, make sure we don't overwrite ourselves
	if ( rLeftCapSize.m_dx > (rLineSize.m_dx / 2.0))
		rLeftCapSize.m_dx	= rLineSize.m_dx / 2.0;

	R2dTransform rLeftCapTransform = rTransform;	
	if (pDS) m_pLeftCap->Render(*pDS, rLeftCapSize, rLeftCapTransform, rRender, rMonoColor);
	if (pPath) m_pLeftCap->GetOutlinePath(*pPath, rLeftCapSize, rLeftCapTransform);
}

// ****************************************************************************
//
// Function Name:			RPsd3RuledLineGraphic::RenderRightCap()
//
// Description:			Draws the right cap of the Ruled Line
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3RuledLineGraphic::RenderRightCap(RDrawingSurface* pDS, RPath* pPath, const RRealSize& rLineSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor) const
{
	if (m_pRightCap)	
	{
		// The right cap exists, so render it
		RRealSize rRightCapSize;
		rRightCapSize.m_dy = rLineSize.m_dy;
		rRightCapSize.m_dx = rLineSize.m_dy * m_pRightCap->GetAspectRatio();

		//	If the line is real small AND the line is not that large, make sure we don't overwrite ourselves
		if ( rRightCapSize.m_dx > (rLineSize.m_dx / 2.0))
			rRightCapSize.m_dx	= rLineSize.m_dx / 2.0;

		R2dTransform rRightCapTransform = rTransform;
		rRightCapTransform.PreTranslate((YRealDimension)(rLineSize.m_dx - rRightCapSize.m_dx), 0.0);
		
		if (pDS) m_pRightCap->Render(*pDS, rRightCapSize, rRightCapTransform, rRender, rMonoColor);
		if (pPath) m_pRightCap->GetOutlinePath(*pPath, rRightCapSize, rRightCapTransform);
	}
	else if (m_fMirrorCaps)
	{
		// The right cap does not exist, so render a mirror image of m_pLeftCap
		RRealSize rLeftCapSize;
		rLeftCapSize.m_dy = rLineSize.m_dy;
		rLeftCapSize.m_dx = rLineSize.m_dy * m_pLeftCap->GetAspectRatio();

		//	If the line is real small AND the line is not that large, make sure we don't overwrite ourselves
		if ( rLeftCapSize.m_dx > (rLineSize.m_dx / 2.0))
			rLeftCapSize.m_dx	= rLineSize.m_dx / 2.0;

		R2dTransform rLeftCapTransform = rTransform;
		rLeftCapTransform.PreTranslate((YRealDimension)rLineSize.m_dx, 0.0);
		rLeftCapTransform.PreScale(-1.0, 1.0);
		
		if (pDS) m_pLeftCap->Render(*pDS, rLeftCapSize, rLeftCapTransform, rRender, rMonoColor);
		if (pPath) m_pLeftCap->GetOutlinePath(*pPath, rLeftCapSize, rLeftCapTransform);
	}
	else
	{
		// Do not render either the right cap or a mirror image of m_pLeftCap
		return;
	}
}

// ****************************************************************************
//
// Function Name:			RPsd3RuledLineGraphic::RenderRails()
//
// Description:			Draws the rails of the Ruled Line
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3RuledLineGraphic::RenderRails(RDrawingSurface* pDS, RPath* pPath, const RRealSize& rLineSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor) const
{
	//	Make sure we have a rail (is this necessary?)
	if ( !m_pRail )
		return;

	//	Compute the inverse of the transform
	R2dTransform	invTransform( rTransform );
	invTransform.Invert( );
	YRealDimension	xScale;
	YRealDimension	yScale;
	YAngle			rotation;
	rTransform.Decompose( rotation, xScale, yScale );

	//	Compute the size of the right and left caps respectively (if they exist)
	YRealDimension	leftCapSize		= (m_pLeftCap!=NULL)?	rLineSize.m_dy * m_pLeftCap->GetAspectRatio( ) : 0.0;
	YRealDimension	rightCapSize	= (m_pRightCap!=NULL)?	rLineSize.m_dy * m_pRightCap->GetAspectRatio( )
														: ((m_fMirrorCaps)? leftCapSize : 0.0 );

	YRealDimension	halfLine	= rLineSize.m_dx / 2.0;
	if ( leftCapSize > halfLine )
		leftCapSize	= halfLine;
	if ( rightCapSize > halfLine)
		rightCapSize = halfLine;

	//	Count the number of rails needed
	YRealDimension yVariableLength( rLineSize.m_dx - leftCapSize - rightCapSize );
	YRealDimension	yNominalXDimension( rLineSize.m_dy * m_pRail->GetAspectRatio() );
	sLONG				sRailCount = ::GetRailCount(yVariableLength, yNominalXDimension );
	//
	//	If no rails are needed, we are done
	if (sRailCount == 0) return;

	// Compute the new rail sizes (the small and large ones)
	RRealSize	smallRailSize( yVariableLength / (YFloatType)sRailCount, rLineSize.m_dy );
	smallRailSize	*= rTransform;
	smallRailSize	= RRealSize( floor( smallRailSize.m_dx ), floor( smallRailSize.m_dy ) );
	RRealSize	largeRailSize( smallRailSize.m_dx+1.0, smallRailSize.m_dy+1.0 );
	//
	//	Re-expand to logical units
	smallRailSize	*= invTransform;
	largeRailSize	*= invTransform;
	//
	//	Reset the y dimension
	smallRailSize.m_dy = largeRailSize.m_dy = rLineSize.m_dy;

	//	What is the excess
	YIntDimension	smallSize	= sRailCount;
	YIntDimension	increment	= static_cast<YIntDimension>( (yVariableLength - (smallRailSize.m_dx * sRailCount)) * xScale) + 1;
	YRealDimension	xStartOffset	= 0.0;
	YIntDimension	excessCounter	= 0;

	TpsAssert( smallSize>=0, "Extra space is negative" );

	if (m_pLeftCap)
		xStartOffset	+= leftCapSize;
	
	RRealSize	rRailSize( rLineSize );
	for (int nRail = 0; nRail < sRailCount-1; ++nRail)
	{
		//	Create a transform and offset it to the proper location
		R2dTransform	renderTransform( rTransform );
		renderTransform.PreTranslate( xStartOffset, 0 );

		//	Add the extra to the excessCounter
		excessCounter	+= increment;
		if ( excessCounter >= smallSize )
		{
			rRailSize		= largeRailSize;//smallRailSize;
			excessCounter	-= smallSize;
		}
		else
			rRailSize	= smallRailSize;//largeRailSize;

		if (pDS) m_pRail->Render(*pDS, rRailSize, renderTransform, rRender, rMonoColor);
		if (pPath) m_pRail->GetOutlinePath(*pPath, rRailSize, renderTransform);

		xStartOffset	+= rRailSize.m_dx;
	}

	//	Render the last rail, but make sure it is large enough
	R2dTransform	tmpTransform( rTransform );
	tmpTransform.PreTranslate( xStartOffset, 0 );
	rRailSize.m_dx	= yVariableLength + leftCapSize - xStartOffset;
	if (pDS) m_pRail->Render(*pDS, rRailSize, tmpTransform, rRender, rMonoColor);
	if (pPath) m_pRail->GetOutlinePath(*pPath, rRailSize, tmpTransform);
}

// ****************************************************************************
//
// Function Name:			RPsd3RuledLineGraphic::Undefine()
//
// Description:			
//
// Returns:					
//	
// Exceptions:				
//
// ****************************************************************************
void RPsd3RuledLineGraphic::Undefine()
{
	delete m_pLeftCap;
	m_pLeftCap = NULL;
	delete m_pRightCap;
	m_pRightCap = NULL;
	delete m_pRail;
	m_pRail = NULL;
	m_fMirrorCaps = FALSE;
}

// ****************************************************************************
//
// Function Name:		RPsd3RuledLineGraphic::GetOutlinePath( )
//
// Description:		Compute and return the path the encompases the path of this graphic
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RPsd3RuledLineGraphic::GetOutlinePath( RPath& path, const RRealSize& size, const R2dTransform& transform )
{	
	R2dTransform rLineTransform = transform;
	RRealSize rLineSize = size;
	if (!m_fHorizontal)
	{
		rLineSize.m_dx = size.m_dy;
		rLineSize.m_dy = size.m_dx;
		rLineTransform.PreRotateAboutOrigin(::DegreesToRadians(-90.0));
		rLineTransform.PreTranslate(0.0, -rLineSize.m_dy);		
	}

	Render( NULL, &path, rLineSize, rLineTransform, RIntRect(0,0,0,0), RColor(kBlack) );
	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RPsd3RuledLineGraphic::SetHorizontal( )
//
// Description:		Draw this line with a horizontal orientation
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
void RPsd3RuledLineGraphic::SetHorizontal()
{
	m_fHorizontal = TRUE;
}

// ****************************************************************************
//
// Function Name:		RPsd3RuledLineGraphic::SetVertical( )
//
// Description:		Draw this line with a vertical orientation
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
void RPsd3RuledLineGraphic::SetVertical()
{
	m_fHorizontal = FALSE;
}

#ifdef TPSDEBUG
// Debugging support
void RPsd3RuledLineGraphic::Validate() const
{
}
#endif // TPSDEBUG

