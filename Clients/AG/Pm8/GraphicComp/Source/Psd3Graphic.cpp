// ****************************************************************************
//
//  File Name:			Psd3Graphic.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Definition of the RPsd3Graphic class
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/GraphicComp/Source/Psd3Graphic.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GraphicIncludes.h"

ASSERTNAME

#include "Psd3Graphic.h"
#include "ResourceManager.h"
#include "ApplicationGlobals.h"
#include "Psd3SingleGraphic.h"
#include "Psd3BorderGraphic.h"
#include "Psd3RuledLineGraphic.h"
#include "Psd3BannerGraphic.h"
#include "Psd3TimepieceGraphic.h"
#include "Psd3BackdropGraphic.h"
#include "CGMSingleGraphic.h"


// ****************************************************************************
//
// Function Name:		RPsd3Graphic::RPsd3Graphic()
//
// Description:		Constructor
//
// Returns:				
//
// Exceptions:			
//
// ****************************************************************************
RPsd3Graphic::RPsd3Graphic()
{
	m_pPsd3GraphicData = NULL;
	m_uPsd3GraphicDataSize = 0;
	m_fPsd3Monochrome = FALSE;
}

// ****************************************************************************
//
// Function Name:		RPsd3Graphic::~RPsd3Graphic( )
//
// Description:		Destructor
//
// Returns:				
//
// Exceptions:			
//
// ****************************************************************************
RPsd3Graphic::~RPsd3Graphic( )
{
	delete [] m_pPsd3GraphicData;
}

// ****************************************************************************
//
// Function Name:		RPsd3Graphic::Load( )
//
// Description:		Loads a Psd3 format graphic and saves a pointer to the 
//							raw data
//
// Returns:				TRUE if it is a valid graphic, FALSE otherwise.
//
// Exceptions:			None
//
// ****************************************************************************
BOOLEAN RPsd3Graphic::Load(const uBYTE* pGraphicData, uLONG uGraphicDataSize, BOOLEAN fMonochrome)
{
	delete [] m_pPsd3GraphicData;
	m_pPsd3GraphicData = new uBYTE[uGraphicDataSize];
	memcpy(m_pPsd3GraphicData, pGraphicData, uGraphicDataSize);
	m_uPsd3GraphicDataSize = uGraphicDataSize;
	m_fPsd3Monochrome = fMonochrome;

	// Initialize the graphic
	return Initialize(m_pPsd3GraphicData, GetAdjustLineWidth(), m_fPsd3Monochrome);
}

// ****************************************************************************
//
// Function Name:		RPsd3Graphic::Load( )
//
// Description:		Loads a Psd3 format graphic and saves a pointer to the 
//							raw data
//
// Returns:				TRUE if it is a valid graphic, FALSE otherwise.
//
// Exceptions:			None
//
// ****************************************************************************
BOOLEAN RPsd3Graphic::Load(YResourceId hId)
{
	BOOLEAN fResult = FALSE;
	
	YResource hData = NULL;	
	uBYTE* pData = NULL;
	try
	{
		YResourceType hType;
		SetResourceType(hType, 'P','H','D','R');
		hData = GetResourceManager().GetResource(hType, hId);
		if (!hData) throw ::kResource;
		
		// Try to load the graphic
		tmemLockNativeResource(hData);
		pData = reinterpret_cast<uBYTE*>(tmemDeRefNativeResource(hData));		
		
		RPsd3GraphicInfo* pGraphicInfo = reinterpret_cast<RPsd3GraphicInfo*>(pData + sizeof(RPsd3GraphicLibraryHeader));
		uBYTE* pGraphicData = pData + sizeof(RPsd3GraphicLibraryHeader) + sizeof(RPsd3GraphicInfo);
		fResult = Load(pGraphicData, pGraphicInfo->m_GraphicSize, pGraphicInfo->m_Mono);

		// Clean up
		tmemUnlockNativeResource(hData);
		tmemPurgeNativeResource(hData);
		GetResourceManager().FreeResource(hData);
	}
	catch(YException)
	{
		// Clean up
		if (pData)
		{
			tmemUnlockNativeResource(hData);
			tmemPurgeNativeResource(hData);
		}
		if (hData) GetResourceManager().FreeResource(hData);
		
		throw;
	}

	return fResult;
}

// ****************************************************************************
//
// Function Name:		RPsd3Graphic::ValidateGraphic( )
//
// Description:		Runs a series of checks to determine if the given graphic
//							is a valid Psd3 Graphic.
//
// Returns:				TRUE if it is a valid graphic, FALSE otherwise.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RPsd3Graphic::ValidateGraphic(const uBYTE*)
{
	// Default value
	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RPsd3Graphic::Read( )
//
// Description:		Streams a Psd3 graphic in from archive storage
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
void RPsd3Graphic::Read(RArchive& rArchive)
{
	uBYTE	ubDummy;
	rArchive >> m_uPsd3GraphicDataSize;
	rArchive	>> ubDummy;
	rArchive >> m_fPsd3Monochrome;

	m_pPsd3GraphicData = new uBYTE[m_uPsd3GraphicDataSize];
	rArchive.Read(m_uPsd3GraphicDataSize, m_pPsd3GraphicData);

	// Initialize the graphic
	Initialize(m_pPsd3GraphicData, GetAdjustLineWidth(), m_fPsd3Monochrome);
}

// ****************************************************************************
//
// Function Name:		RPsd3Graphic::ValidateGraphic( )
//
// Description:		Streams a Psd3 graphic out to archive storage
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
void RPsd3Graphic::Write(RArchive& rArchive)
{	
	uBYTE	ubDummy	= 0;
	rArchive << m_uPsd3GraphicDataSize;
	rArchive	<< ubDummy;
	rArchive << m_fPsd3Monochrome;
	rArchive.Write(m_uPsd3GraphicDataSize, m_pPsd3GraphicData);
}
// ****************************************************************************
//
// Function Name:		RPsd3Graphic::CreateNewGraphic( )
//
// Description:		Creates a new graphic
//
// Returns:				new graphic
//
// Exceptions:			kMemory
//
// ****************************************************************************
//
RPsd3Graphic* RPsd3Graphic::CreateNewGraphic( RGraphicInterface::EGraphicType graphicType )
	{
	switch( graphicType )
		{
		case RGraphicInterface::kSquare :
		case RGraphicInterface::kRow :
		case RGraphicInterface::kColumn :
		case RGraphicInterface::kSeal :
			return new RPsd3SingleGraphic;

		case RGraphicInterface::kBorder :
			return new RPsd3BorderGraphic;

		case RGraphicInterface::kLine :
			return new RPsd3RuledLineGraphic;

		case RGraphicInterface::kBackdrop :
			return new RPsd3BackdropGraphic;

		case RGraphicInterface::kHorzBannerBackdrop :
			return new RPsd3HorizontalBannerGraphic;

		case RGraphicInterface::kVertBannerBackdrop :
			return new RPsd3VerticalBannerGraphic;

		case RGraphicInterface::kTiledBannerBackdrop :
			return new RPsd3TiledBackdropGraphic;

		case RGraphicInterface::kTimepiece :
			return new RPsd3TimepieceGraphic;

		case RGraphicInterface::kCGM:
			return new RCGMSingleGraphic;

		default :
			TpsAssertAlways( "Invalid graphic type." );
		}

	return NULL;
	}
