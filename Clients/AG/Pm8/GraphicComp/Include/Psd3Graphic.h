// ****************************************************************************
//
//  File Name:			Psd3Graphic.h
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RPsd3Graphic class
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
//  $Logfile:: /PM8/GraphicComp/Include/Psd3Graphic.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _PSD3GRAPHIC_H_
#define _PSD3GRAPHIC_H_

#ifndef _PSD3GRAPHICFILEFORMAT_H_
#include "Psd3GraphicFileFormat.h"
#endif

#ifndef	_CHUNKY_STORAGE_H_
#include	"ChunkyStorage.h"
#endif

#include "GraphicInterface.h"

class RPsd3Graphic
{
// Member functions
public:
	// Construction, Destruction & Initialization
	RPsd3Graphic();	
	virtual ~RPsd3Graphic();

	// Operations
	BOOLEAN								Load(const uBYTE* pGraphicData, uLONG uGraphicDataSize, BOOLEAN fMonochrome = FALSE);
	BOOLEAN								Load(YResourceId nId);
	virtual BOOLEAN					Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth = FALSE, BOOLEAN fMonochrome = FALSE) = 0;
	virtual BOOLEAN					ValidateGraphic(const uBYTE* pGraphicData);
	
	// Archive support
	virtual void						Read(RArchive& rArchive);
	virtual void						Write(RArchive& rArchive);

	// Accessors
	virtual BOOLEAN					ContainsGradientData() = 0;
	virtual BOOLEAN					GetAdjustLineWidth() = 0;
	virtual YChunkTag					GetChunkStorageTag() = 0;

	uBYTE*								GetGraphicData() const {return m_pPsd3GraphicData;}
	uLONG									GetGraphicDataSize() const {return m_uPsd3GraphicDataSize;}	
	BOOLEAN								GetMonochrome() const {return m_fPsd3Monochrome;}

	static RPsd3Graphic*				CreateNewGraphic( RGraphicInterface::EGraphicType graphicType );	

// Data members
protected:
	uBYTE*								m_pPsd3GraphicData;
	uLONG									m_uPsd3GraphicDataSize;	
	BOOLEAN								m_fPsd3Monochrome;
};

#endif // _PSD3GRAPHIC_H_