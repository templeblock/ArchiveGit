// *****************************************************************************
//
// File name:			RailGraphicUtility.h
//
// Project:				Renaissance Framework
//
// Author:				G. Brown
//
// Description:		Declaration of the RRuledLineGraphic class.
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

#ifndef _RAILGRAPHICUTILITY_H_
#define _RAILGRAPHICUTILITY_H_

inline uLONG GetRailCount(YRealDimension yTotalVariableLength, YRealDimension yRailLength)
{
	// Make sure that we have enough room for the rails
	if (yTotalVariableLength <= 0) return 0;
	
	// Compute the best number of rails to display within the given area
	// by comparing the aspect ratio of n stretched rails to n + 1 shrunk rails
	uLONG uNumberOfRails = (uLONG)(yTotalVariableLength / yRailLength);
	YRealDimension yLengthA = yTotalVariableLength / (YRealDimension)uNumberOfRails;
	YRealDimension yLengthB = yTotalVariableLength / (YRealDimension)(uNumberOfRails + 1);

	if (fabs(yLengthA - yRailLength) < fabs(yLengthB - yRailLength))
		return uNumberOfRails;
	else
		return uNumberOfRails + 1;
}

// ****************************************************************************
//
// Function Name:			FlipX()
//
// Description:			
//
// Returns:					
//
// Exceptions:				None
//
// ****************************************************************************
inline void FlipX(const RRealSize& rElementSize, R2dTransform& rTransform)
{
	rTransform.PreTranslate(rElementSize.m_dx, 0.0);
	rTransform.PreScale(-1.0, 1.0);
}

// ****************************************************************************
//
// Function Name:			FlipY()
//
// Description:			
//
// Returns:					
//
// Exceptions:				None
//
// ****************************************************************************
inline void FlipY(const RRealSize& rElementSize, R2dTransform& rTransform)
{
	rTransform.PreTranslate(0.0, rElementSize.m_dy);
	rTransform.PreScale(1.0, -1.0);
}

// ****************************************************************************
//
// Function Name:			GetXSize()
//
// Description:			
//
// Returns:					
//
// Exceptions:				None
//
// ****************************************************************************
inline RRealSize GetXSize(const RSingleGraphic* pGraphic, YRealDimension yFixedDimension)
{
	return (pGraphic) ? RRealSize(yFixedDimension * pGraphic->GetAspectRatio(), yFixedDimension) : RRealSize(0.0, 0.0);
}

// ****************************************************************************
//
// Function Name:			GetYSize()
//
// Description:			
//
// Returns:					
//
// Exceptions:				None
//
// ****************************************************************************
inline RRealSize GetYSize(const RSingleGraphic* pGraphic, YRealDimension yFixedDimension)
{
	return (pGraphic) ? RRealSize(yFixedDimension, yFixedDimension / pGraphic->GetAspectRatio()) : RRealSize(0.0, 0.0);
}

#endif //_RAILGRAPHICUTILITY_H_