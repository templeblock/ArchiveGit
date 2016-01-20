// ****************************************************************************
//
//  File Name:			Psd3GraphicInterfaceImp.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RPsd3GraphicInterfaceImp class
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
// ****************************************************************************

#ifndef	_PSD3GRAPHICINTERFACEIMP_H_
#define	_PSD3GRAPHICINTERFACEIMP_H_

#include	"Psd3GraphicInterface.h"
#include "Psd3GraphicLibrary.h"
#include "FileStream.h"

class RGraphicView;
class RGraphicDocument;

class RPsd3GraphicInterfaceImp : public RPsd3GraphicInterface
{
//	Construction & Destruction
public :
												RPsd3GraphicInterfaceImp( const RGraphicView* pView );
	static RInterface*					CreateInterface( const RComponentView* pView );

//	Operations
public :
	virtual BOOLEAN						OpenLibrary(const RMBCString& rLibraryPath);
	virtual char**							GetGraphicNames(uWORD& nGraphicCount) const;
	virtual BOOLEAN						LoadGraphic(const char* pGraphicName);	

	virtual BOOLEAN						Load(uBYTE* pRawData, uLONG uRawDataLength, BOOLEAN fMonochrome, RGraphicInterface::EGraphicType graphicType);

// Data members
private:
	RGraphicDocument*						m_pGraphicDocument;
	RFileStream								m_rLibraryStream;
	RPsd3GraphicLibrary					m_rLibrary;
	BOOLEAN									m_fLibraryOpen;
} ;

#endif	//	_PSD3GRAPHICINTERFACEIMP_H_
