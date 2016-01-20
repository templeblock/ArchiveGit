// *****************************************************************************
//
// File name:			Psd3TimepieceGraphic.cpp
//	
// Project:				Renaissance Framework
//
// Author:				G. Brown
//
// Description:		Definition of the RPsd3TimepieceGraphic class.
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

#include "Psd3TimepieceGraphic.h"

const RTime kDefaultTime(00, 00, TRUE);

// ****************************************************************************
//
// Function Name:			RPsd3TimepieceGraphic::RPsd3TimepieceGraphic()
//
// Description:			Constructor
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
RPsd3TimepieceGraphic::RPsd3TimepieceGraphic()
:	m_pClockGraphic(NULL),
	m_pBigHandGraphic(NULL),
	m_pLittleHandGraphic(NULL),
	m_rTime(kDefaultTime)
{
}

// ****************************************************************************
//
// Function Name:			RPsd3TimepieceGraphic::~RPsd3TimepieceGraphic()
//
// Description:			Destructor
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
RPsd3TimepieceGraphic::~RPsd3TimepieceGraphic()
{
	delete m_pClockGraphic;
	delete m_pBigHandGraphic;
	delete m_pLittleHandGraphic;
}

// ****************************************************************************
//
// Function Name:			RPsd3TimepieceGraphic::Initialize()
//
// Description:			Initializes a graphic (does not store raw data)
//
// Returns:					TRUE if successful, FALSE otherwise
//
// Exceptions:				None
//
// ****************************************************************************
BOOLEAN RPsd3TimepieceGraphic::Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth, BOOLEAN fMonochrome)
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
		
		// Make sure we have a valid timepiece
		if (pHeader->m_PartsList != kPsd3AnalogTimepiecePartsList && pHeader->m_PartsList != kPsd3DigitalTimepiecePartsList) throw;
		
		// Rect
		{
			const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;				
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome)) throw;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the graphic

			// Compute the m_rClockRect offsets here:
			// For an analog timepiece, the center of rotation of the clock hands
			// is m_rClockRect.m_Left, m_rClockRect.m_Top.  For a digital timepiece,
			// the rect in which to render the digits is m_rClockRect.
			// Each member of m_rClockRect is given as a percentage of the size
			// of the entire clock so we can render timepieces of arbitrary size
			RRealSize rGraphicSize = pGraphic->GetGraphicSize();
			RRealRect rFirstBox = pGraphic->GetFirstBox();

			// Calculate the Top/Left offset percentages
			m_rClockRect.m_Left = rFirstBox.m_Right / rGraphicSize.m_dx;
			m_rClockRect.m_Top = 1.0 - (rFirstBox.m_Bottom / rGraphicSize.m_dy);
			
			// Calculate the width and height percentages
			m_rClockRect.m_Right = (rFirstBox.m_Left - rFirstBox.m_Right) / rGraphicSize.m_dx;
			m_rClockRect.m_Bottom = (rFirstBox.m_Bottom - rFirstBox.m_Top) / rGraphicSize.m_dy;
		}

		// Clock
		{
			const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;				
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome)) throw;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the graphic
			m_pClockGraphic = pGraphic;
		}

		// If this is a digital timepiece, we don't load the hands
		if (pHeader->m_PartsList == kPsd3DigitalTimepiecePartsList) return TRUE;
				
		// Little hand
		{
			const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;				
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome)) throw;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the graphic
			m_pLittleHandGraphic = pGraphic;
		}

		// Big hand
		{
			const uBYTE* pCurrentGraphic = pGraphicData + sGraphicOffset;
			RPsd3SingleGraphic* pGraphic = new RPsd3SingleGraphic;				
			if (!pGraphic->Initialize(pCurrentGraphic, fAdjustLineWidth, fMonochrome)) throw;
			sGraphicOffset += *((uLONG*)pCurrentGraphic); // The size of the graphic
			m_pBigHandGraphic = pGraphic;
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
// Function Name:			RPsd3TimepieceGraphic::ContainsGradientData()
//
// Description:			Returns TRUE if this graphic contains gradient data
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
BOOLEAN RPsd3TimepieceGraphic::ContainsGradientData()
{
	if (m_pClockGraphic && m_pClockGraphic->ContainsGradientData()) return TRUE;
	else if (m_pBigHandGraphic && m_pBigHandGraphic->ContainsGradientData()) return TRUE;
	else if (m_pLittleHandGraphic && m_pLittleHandGraphic->ContainsGradientData()) return TRUE;
	else return FALSE;
}

// ****************************************************************************
//
// Function Name:			RPsd3TimepieceGraphic::Render()
//
// Description:			Draws the Backdrop
//
// Returns:					Nothing
//
// Exceptions:				None
//
// ****************************************************************************
void RPsd3TimepieceGraphic::Render(RDrawingSurface& rDrawingSurface, const RRealSize& rSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor, BOOLEAN)
{	
	const YAngle	kRadiansPerHour =		kPI / 6.0;
	const YAngle	kRadiansPerMinute =	kPI / 30.0;
	const uBYTE		kMinutesPerHour =		60;

	// Render the clock
	m_pClockGraphic->Render(rDrawingSurface, rSize, rTransform, rRender, rMonoColor);
	if (IsDigital()) return;
	
	// Set the hand transforms based on the time
	RRealPoint rCenterOfRotation(rSize.m_dx * m_rClockRect.m_Left, rSize.m_dy * m_rClockRect.m_Top);

	// The little hand defaults to 6 o'clock, so adjust accordingly
	R2dTransform rLittleHandTransform = rTransform;
	
	// Set the hour hand's rotation
	YAngle yLittleHandRotation = (YAngle)((6.0 - (YFloatType)m_rTime.GetHour()) * kRadiansPerHour);
	
	// Adjust the hour hand for minutes past the hour
	yLittleHandRotation -= (YAngle)((YFloatType)m_rTime.GetMinutes() * kRadiansPerHour / (YFloatType)kMinutesPerHour);

	// Apply the rotation
	rLittleHandTransform.PreRotateAboutPoint(rCenterOfRotation.m_x, rCenterOfRotation.m_y, yLittleHandRotation);

	// The big hand defaults to 12 o'clock, so adjust accordingly
	R2dTransform rBigHandTransform = rTransform;
	YAngle yBigHandRotation = -(YAngle)((YFloatType)m_rTime.GetMinutes() * kRadiansPerMinute);
	rBigHandTransform.PreRotateAboutPoint(rCenterOfRotation.m_x, rCenterOfRotation.m_y, yBigHandRotation);
	
	m_pBigHandGraphic->Render(rDrawingSurface, rSize, rBigHandTransform, rRender, rMonoColor);
	m_pLittleHandGraphic->Render(rDrawingSurface, rSize, rLittleHandTransform, rRender, rMonoColor);
}

// ****************************************************************************
//
// Function Name:		RPsd3TimepieceGraphic::GetOutlinePath( )
//
// Description:		Compute and return the path the encompases the path of this graphic
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
BOOLEAN RPsd3TimepieceGraphic::GetOutlinePath( RPath& path, const RRealSize& size, const R2dTransform& transform )
{
	if (m_pClockGraphic)
	{
		// Get the path of the clock (not including the hands)
		m_pClockGraphic->GetOutlinePath(path, size, transform);
		return TRUE;
	}
	else
	{
		TpsAssertAlways("No clock graphic to outline.");
		return FALSE;
	}
}

// ****************************************************************************
//
// Function Name:			RPsd3TimepieceGraphic::SetTime()
//
// Description:			Sets the time
//
// Returns:					
//
// Exceptions:				
//
// ****************************************************************************
void RPsd3TimepieceGraphic::SetTime(const RTime& rTime)
{
	m_rTime = rTime;
}

// ****************************************************************************
//
// Function Name:			RPsd3TimepieceGraphic::GetTime()
//
// Description:			Gets the time
//
// Returns:					
//
// Exceptions:				
//
// ****************************************************************************
const RTime& RPsd3TimepieceGraphic::GetTime() const
{
	return m_rTime;
}

// ****************************************************************************
//
// Function Name:			RPsd3TimepieceGraphic::IsDigital()
//
// Description:			
//
// Returns:					
//
// Exceptions:				
//
// ****************************************************************************
BOOLEAN RPsd3TimepieceGraphic::IsDigital()
{
	return (m_pBigHandGraphic == NULL && m_pLittleHandGraphic == NULL) ? TRUE : FALSE;
}

// ****************************************************************************
//
// Function Name:			RPsd3TimepieceGraphic::GetClockRect()
//
// Description:			
//
// Returns:					
//	
// Exceptions:				
//
// ****************************************************************************
const RRealRect& RPsd3TimepieceGraphic::GetClockRect() const
{
	return m_rClockRect;
}

// ****************************************************************************
//
// Function Name:			RPsd3TimepieceGraphic::Undefine()
//
// Description:			
//
// Returns:					
//	
// Exceptions:				
//
// ****************************************************************************
void RPsd3TimepieceGraphic::Undefine()
{
	m_rClockRect = RRealRect(0.0, 0.0, 0.0, 0.0);

	delete m_pClockGraphic;
	m_pClockGraphic = NULL;
	delete m_pBigHandGraphic;
	m_pBigHandGraphic = NULL;
	delete m_pLittleHandGraphic;
	m_pLittleHandGraphic = NULL;
}

