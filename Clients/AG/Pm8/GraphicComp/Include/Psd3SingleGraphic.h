// ****************************************************************************
//
//  File Name:			Psd3SingleGraphic.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RPsd3SingleGraphic class
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
//  $Logfile:: /PM8/GraphicComp/Include/Psd3SingleGraphic.h                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _PSD3SINGLEGRAPHIC_H_
#define _PSD3SINGLEGRAPHIC_H_

#ifndef _SINGLEGRAPHIC_H_
#include "SingleGraphic.h"
#endif

#ifndef _PSD3GRAPHIC_H_
#include "Psd3Graphic.h"
#endif

typedef uBYTE YPointPackType;
const YPointPackType kByte = 0;
const YPointPackType kShort = 4;
const YPointPackType kNibble = 8;

const YChunkTag kPsd3SingleGraphicChunkID = 0x02160001;

class RPsd3SingleGraphic : public RSingleGraphic, public RPsd3Graphic
{
public:
	// Construction, Destruction & Initialization
	RPsd3SingleGraphic();
	RPsd3SingleGraphic(const RPsd3SingleGraphic &rhs);
	virtual ~RPsd3SingleGraphic();

	// Operations	
	virtual BOOLEAN					Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth = FALSE, BOOLEAN fMonochrome = FALSE);
	virtual BOOLEAN					ValidateGraphic(const uBYTE* pGraphicData);
	virtual BOOLEAN					ContainsGradientData();
	virtual BOOLEAN					GetAdjustLineWidth() {return TRUE;}
	virtual YChunkTag					GetChunkStorageTag() {return kPsd3SingleGraphicChunkID;}

private :
	// Member data
	RIntPoint							m_CurrentPosition;	
	BOOLEAN								m_fContainsGradientData;

	// Member functions
	void									UnpackPoints( uBYTE const*& pGraphicElements, YPointPackType pointPackType, YPointCount pointCount );
	BOOLEAN								ConvertGraphicElements( const uBYTE* pGraphicElements, short elementCount );
	YFillMethod							GetFillMethod( uBYTE info ) const;
	YPointPackType						GetPointPackType( uBYTE info ) const;
	void									SetupFillMethod( YFillMethod& method, BOOLEAN& fLastWasGradient, const RSolidColor& rgb, YGradientIndex nidxGradient );
};

#endif // _PSD3SINGLEGRAPHIC_H_