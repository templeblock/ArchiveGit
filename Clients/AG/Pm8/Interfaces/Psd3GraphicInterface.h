// ****************************************************************************
//
//  File Name:			Psd3GraphicInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RPsd3GraphicInterface class
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
//  $Logfile:: /PM8/Interfaces/Psd3GraphicInterface.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_PSD3GRAPHICINTERFACE_H_
#define	_PSD3GRAPHICINTERFACE_H_

#ifndef	_INTERFACE_H_
#include	"Interface.h"
#endif

#ifndef	_GRAPHICINTERFACE_H_
#include "GraphicInterface.h"
#endif

class RStorage;
class RStream;
class RPsd3Graphic;

const	YInterfaceId	kPsd3GraphicInterfaceId = 92;

// ****************************************************************************
//
//  Class Name:	RPsd3GraphicInterface
//
//  Description:	A pure virtual class interfacing with a graphic
//
// ****************************************************************************
//
class RPsd3GraphicInterface : public RInterface
{
//	Construction & Destruction
public :
												RPsd3GraphicInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

//	Operations
public :	
	virtual BOOLEAN						OpenLibrary(const RMBCString& rLibraryPath) = 0;
	virtual char**							GetGraphicNames(uWORD& nGraphicCount) const = 0;	
	virtual BOOLEAN						LoadGraphic(const char* pGraphicName) = 0;	

	virtual BOOLEAN						Load(uBYTE* pRawData, uLONG uRawDataLength, BOOLEAN fMonochrome, RGraphicInterface::EGraphicType graphicType) = 0;
};

#endif	//_PSD3GRAPHICINTERFACE_H_
