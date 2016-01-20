// ****************************************************************************
//
//  File Name:			ObjectSize.cpp
//
//  Project:			Renaissance Application
//
//  Author:				R. Hood
//
//  Description:		Definition of the RObjectSize class
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
//  $Logfile:: /PM8/App/ObjectSize.cpp                                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:08p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "stdafx.h"
#include "ObjectSize.h"
#include "ComponentTypes.h"

//	Define object size rules.
//	Use object sizes from PSD 4 to calculate aspect ratio since they do not change for new version.
YRealDimension	RObjectSize::m_yObjectSizes[RObjectSize::kNumObjectTypes ][ RObjectSize::kNumObjectSizeTypes] = 
{
	//						//	kDefaultSizePortrait		//	kDefaultSizeLandscape
	//	kAspectRatio,				//	kMinSizePortrait				//	kMinSizeLandscape
	//												//	kMaxObjectSize					//	kMaxObjectSizeBanner
	{	1.75/1.75,		.31,		.03,		30.,		.31,			.025,		416.		},	//	kSquare
	{	6./1.,			.83,		.26,		30.,		.14,			.03,		416.		},	//	kRow
	{	1./6.,			.14,		.04,		30.,		.83,			.19,		416.		},	//	kCollumn
	{	7.26/9.9,		1.0,		1.0,		30.,		1.0,			1.0,		416.		},	//	kBackdrop
	{	5./1.,			.96,		.05,		30.,		.19,			.04,		416.		},	//	kHeadline
	{	5./1.,			.96,		.05,		30.,		.19,			.04,		10000.	},	//	kSpecialHeadline
	{	1./5.,			.19,		.05,		30.,		.83,			.04,		416.		},	//	kVerticalHeadline
	{	1./5.,			.19,		.05,		30.,		.83,			.04,		10000.	},	//	kVerticalSpecialHeadline
	{	4./2.,			.62,		.05,		30.,		.31,			.04,		416.		},	//	kTextBlock
	{	4./2.,			.62,		.05,		30.,		.31,			.04,		416.		},	//	kReturnAddressBlock
	{	3.75/.375,		.52,		.13,		30.,		.05,			.02,		416.		},	//	kHorizontalRuledLine
	{	.375/3.75,		.05,		.02,		30.,		.52,			.13,		416.		},	//	kVerticalRuledLine
	{	1.875/1.875,	.31,		.03,		30.,		.31,			.025,		416.		},	//	kSeal
	{	2./.5,			.28,		.06,		30.,		.07,			.01,		416.		},	//	k1Signature
	{	2./.5,			.55,		.12,		30.,		.14,			.02,		416.		},	//	k2Signature	REVIEW esv 9/4/96 - What is real value for this?
	{	2./.5,			.83,		.18,		30.,		.21,			.03,		416.		},	//	k3Signature REVIEW esv 9/4/96 - What is real value for this?
	{	2./.5,			.83,		.24,		30.,		.21,			.04,		416.		},	//	k4Signature REVIEW esv 9/4/96 - What is real value for this?
	{	3.08/3.08,		.43,		.14,		30.,		.31,			.06,		416.		},	//	kBorder	-	use the value for miniborder since there is no way to tell the difference and size of borders is handled elsewhere
	{	3.08/3.08,		.43,		.14,		30.,		.31,			.06,		416.		},	//	kMiniBorder
	{	7.26/9.9,		1.0,		1.0,		30.,		1.0,			1.0,		416.		},	//	kCertificateBorder
	{	2.08/2.08,		.25,		.25,		30.,		.25,			.25,		416.		},	//	kImportedObject
	{	2.08/2.08,		.25,		.25,		30.,		.25,			.25,		416.		},	//	kImage
	{	2.08/2.08,		.25,		.25,		30.,		.25,			.25,		416.		},	//	kPhoto
	{	2.5/2.5,			.25,		.25,		30.,		.25,			.25,		416.		},	//	kMiniBackdrop
	{	1.6/1.6,			.25,		.25,		30.,		.25,			.25,		416.		},	//	kTileBackdrop
	{	1.75/1.75,		.31,		.03,		30.,		.31,			.025,		416.		},	//	kTimepiece
	{	1.75/1.75,		.31,		.03,		30.,		.31,			.025,		416.		},	//	kDecorativeNumber
	{	1.75/1.75,		.31,		.03,		30.,		.31,			.025,		416.		},	//	kInitialCap
	{	3.00/3.00,		.31,		.03,		30.,		.31,			.025,		416.		},	// kNonstandard
	{	1.75/1.75,		.31,		.03,		30.,		.31,			.025,		416.		},	//	kLogoObject
	{	1.75/1.75,		.31,		.03,		30.,		.31,			.025,		416.		},	//	kGroupObject
	{	1.75/1.75,		.31,		.03,		30.,		.31,			.025,		416.		}	//	kCGMObject
};

// ****************************************************************************
//
//  Function Name:	RObjectSize::GetMinSizePortrait( const RObjectSize::EObjectType eType )
//
//  Description:		return min size for given object type when project is in 
//							portrait orientation
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YRealDimension RObjectSize::GetMinSizePortrait( const RObjectSize::EObjectType eType )
	{
	return m_yObjectSizes[ eType ][ kMinSizePortrait ];
	}


// ****************************************************************************
//
//  Function Name:	RObjectSize::GetMinSizeLandscape( const RObjectSize::EObjectType eType )
//
//  Description:		return min size for given object type when project is in 
//							landscape orientation
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YRealDimension RObjectSize::GetMinSizeLandscape( const RObjectSize::EObjectType eType )
	{
	return m_yObjectSizes[ eType ][ kMinSizeLandscape ];
	}

// ****************************************************************************
//
//  Function Name:	RObjectSize::GetMaxObjectSize( const RObjectSize::EObjectType eType )
//
//  Description:		return max size for given object type for all project types 
//							except banner
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YRealDimension RObjectSize::GetMaxObjectSize( const RObjectSize::EObjectType eType )
	{
	return ::InchesToLogicalUnits( m_yObjectSizes[ eType ][ kMaxObjectSize ] );
	}


// ****************************************************************************
//
//  Function Name:	RObjectSize::GetMaxObjectSizeBanner( const RObjectSize::EObjectType eType )
//
//  Description:		return max size for given object type when project is  
//							a banner
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YRealDimension RObjectSize::GetMaxObjectSizeBanner( const RObjectSize::EObjectType eType )
	{
	return ::InchesToLogicalUnits( m_yObjectSizes[ eType ][ kMaxObjectSizeBanner ] );
	}

// ****************************************************************************
//
//  Function Name:	RObjectSize::GetDefaultSizePortrait( const RObjectSize::EObjectType eType )
//
//  Description:		return min size for given object type when project is in 
//							portrait orientation
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YRealDimension RObjectSize::GetDefaultSizePortrait( const RObjectSize::EObjectType eType )
	{
	return m_yObjectSizes[ eType ][ kDefaultSizePortrait ];
	}


// ****************************************************************************
//
//  Function Name:	RObjectSize::GetAspectRatio( const RObjectSize::EObjectType eType )
//
//  Description:		return apect ratio for given object type 
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YRealDimension RObjectSize::GetAspectRatio( const RObjectSize::EObjectType eType )
	{
	return m_yObjectSizes[ eType ][ kAspectRatio ];
	}


// ****************************************************************************
//
//  Function Name:	RObjectSize::GetDefaultSizeLandscape( const RObjectSize::EObjectType eType )
//
//  Description:		return min size for given object type when project is in 
//							landscape orientation
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YRealDimension RObjectSize::GetDefaultSizeLandscape( const RObjectSize::EObjectType eType )
	{
	return m_yObjectSizes[ eType ][ kDefaultSizeLandscape ];
	}


// ****************************************************************************
//
//  Function Name:	RObjectSize::GetObjectType( const YComponentType& componentType )
//
//  Description:		Return EObjectType for given component type.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RObjectSize::EObjectType RObjectSize::GetObjectType( const YComponentType& componentType )
	{
	RObjectSize::EObjectType eType;

	if ( componentType ==		kSquareGraphicComponent )
		eType = kSquareObject;

	else if ( componentType == kRowGraphicComponent )
		eType = kRowObject;

	else if ( componentType == kColumnGraphicComponent )
		eType = kColumnObject;

	else if ( componentType == kHeadlineComponent )
		eType = kHeadlineObject;

	else if ( componentType == kSpecialHeadlineComponent )
		eType = kSpecialHeadlineObject;

	else if ( componentType == kVerticalHeadlineComponent )
		eType = kVerticalHeadlineObject;

	else if ( componentType == kVerticalSpecialHeadlineComponent )
		eType = kVerticalSpecialHeadlineObject;

	else if ( componentType == kTextComponent )
		eType = kTextBlockObject;

	else if ( componentType == kReturnAddressComponent )
		eType = kReturnAddressObject;

	else if ( componentType == kBackdropGraphicComponent )
		eType = kBackdropObject;

	else if ( componentType == kBorderComponent )
		eType = kBorderObject;

	else if ( componentType == kMiniBorderComponent )
		eType = kMiniBorderObject;

	else if ( componentType == kImageComponent )
		// REVIEW esv 10/14/96 - What size rules should we use for this component?
		eType = kSquareObject;

	else if ( componentType == kHorizontalLineComponent )
		eType = kHorizontalRuledLineObject;

	else if ( componentType == kVerticalLineComponent )
		eType = kVerticalRuledLineObject;

	else if ( componentType == kGroupComponent )
		eType = kGroupObject;

	else if ( componentType == kTimepieceComponent )
		eType = kTimepieceObject;

	else if ( componentType == kSignatureComponent )
		eType = k1SignatureObject;

	else if ( componentType == kCGMGraphicComponent )
		eType = kCGMObject;

	else
		eType = kNonStandardObject;

	return eType;
	}
