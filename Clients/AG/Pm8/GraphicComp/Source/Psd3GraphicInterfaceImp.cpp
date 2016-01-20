//****************************************************************************
//
// File Name:		Psd3GraphicInterfaceImp.cpp
//
// Project:			Renaissance Graphic Component
//
// Author:			G. Brown
//
// Description:	Implementation of the Psd3Graphic Interface
//
// Portability:	Platform Independant
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/GraphicComp/Source/Psd3GraphicInterfaceImp.cpp            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "GraphicIncludes.h"

ASSERTNAME

#include "Psd3GraphicInterfaceImp.h"
#include "GraphicDocument.h"
#include "GraphicView.h"
#include "Psd3GraphicLibrary.h"
#include "ChunkyStorage.h"
#include "Stream.h"

// ****************************************************************************
//
//  Function Name:	RPsd3GraphicInterfaceImp::RPsd3GraphicInterfaceImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPsd3GraphicInterfaceImp::RPsd3GraphicInterfaceImp( const RGraphicView* pGraphicView )
	: RPsd3GraphicInterface( pGraphicView ),
	  m_pGraphicDocument( const_cast<RGraphicDocument*>( pGraphicView->GetGraphicDocument( ) ) ),
	  m_fLibraryOpen(FALSE)
{
}

// ****************************************************************************
//
//  Function Name:	RTimepiecePlacementInterfaceImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* RPsd3GraphicInterfaceImp::CreateInterface( const RComponentView* pView )
{
	TpsAssertIsObject( RGraphicView, pView );
	return new RPsd3GraphicInterfaceImp( static_cast<const RGraphicView*>( pView ) );
}

// ****************************************************************************
//
//  Function Name:	RPsd3GraphicInterfaceImp::Load( )
//
//  Description:		Loads the component with a Psd3Graphic
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
BOOLEAN RPsd3GraphicInterfaceImp::Load(uBYTE* pRawData, uLONG uRawDataLength, BOOLEAN fMonochrome, RGraphicInterface::EGraphicType eGraphicType)
{
	RPsd3Graphic* pGraphic = RPsd3Graphic::CreateNewGraphic(eGraphicType);

	// Load the graphic
	if (pGraphic->Load(pRawData, uRawDataLength, fMonochrome))
	{
		// Set the graphic in the document
		RGraphic* pOldGraphic = m_pGraphicDocument->SetGraphic(dynamic_cast<RGraphic*>(pGraphic));
		delete pOldGraphic;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// ****************************************************************************
//
//  Function Name:	RPsd3GraphicInterfaceImp::OpenLibrary( )
//
//  Description:		Opens a PSD3/4 graphic library
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
BOOLEAN RPsd3GraphicInterfaceImp::OpenLibrary(const RMBCString& rLibraryPath)
{	
	m_fLibraryOpen = FALSE;
	try
	{		
		m_rLibraryStream.Open(rLibraryPath, kRead);
		m_fLibraryOpen = m_rLibrary.OpenLibrary(m_rLibraryStream);
	}
	catch(YException)
	{
	}	
	
	return m_fLibraryOpen;
}

// ****************************************************************************
//
//  Function Name:	RPsd3GraphicInterfaceImp::GetGraphicNames( )
//
//  Description:		Gets the names of available graphics in the library
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
char** RPsd3GraphicInterfaceImp::GetGraphicNames(uWORD& nGraphicCount) const
{
	if (m_fLibraryOpen) 
		return m_rLibrary.GetGraphicNames(nGraphicCount);
	else
		return NULL;
}

// ****************************************************************************
//
//  Function Name:	RPsd3GraphicInterfaceImp::LoadGraphic( )
//
//  Description:		Loads the component with a Psd3Graphic
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
BOOLEAN RPsd3GraphicInterfaceImp::LoadGraphic(const char* pGraphicName)
{
	if (!m_fLibraryOpen) return FALSE;
	
	RPsd3Graphic* pPsd3Graphic = m_rLibrary.GetGraphic(m_rLibraryStream, pGraphicName);
	if (pPsd3Graphic)	
	{		
		RGraphic* pOldGraphic = m_pGraphicDocument->SetGraphic(dynamic_cast<RGraphic*>(pPsd3Graphic));
		delete pOldGraphic;
	}

	return pPsd3Graphic ? TRUE : FALSE;
}

