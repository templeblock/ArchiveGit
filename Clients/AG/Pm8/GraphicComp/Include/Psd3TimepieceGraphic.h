// *****************************************************************************
//
// File name:		Psd3TimepieceGraphic.h
//
// Project:			Renaissance Framework
//
// Author:			G. Brown
//
// Description:	Declaration of the RPsd3RuledLineGraphic class.
//
// Portability:	Platform-independent
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

#ifndef _PSD3TIMEPIECEGRAPHIC_H_
#define _PSD3TIMEPIECEGRAPHIC_H_

#include "Graphic.h"
#include "Psd3Graphic.h"
#include "Psd3SingleGraphic.h"
#include "DateTime.h"

const uWORD kPsd3AnalogTimepiecePartsList =	0x000F;
const uWORD kPsd3DigitalTimepiecePartsList =	0x0003;

const YChunkTag kPsd3TimepieceGraphicChunkID = 0x02160007;

class RPsd3TimepieceGraphic : public RGraphic, public RPsd3Graphic
{
// Member functions
public:
	// Construction/destruction
										RPsd3TimepieceGraphic();
	virtual							~RPsd3TimepieceGraphic();

	// Operations
	virtual BOOLEAN				Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth = FALSE, BOOLEAN fMonochrome = FALSE);
	virtual BOOLEAN				ContainsGradientData();
	virtual void					Render(RDrawingSurface& rDrawingSurface, const RRealSize& rSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& monochromeColor = RSolidColor( kBlack ), BOOLEAN fImageAsMask = FALSE);
	virtual BOOLEAN				GetOutlinePath(RPath& rPath, const RRealSize& rSize, const R2dTransform& rTransform);
	virtual BOOLEAN				IsMonochrome() const { return GetMonochrome(); }
	
	virtual BOOLEAN				GetAdjustLineWidth() {return FALSE;}
	virtual YChunkTag				GetChunkStorageTag() {return kPsd3TimepieceGraphicChunkID;}
	
	// Accessors
	void								SetTime(const RTime& rTime);
	const RTime&					GetTime() const;
	BOOLEAN							IsDigital();
	const RRealRect&				GetClockRect() const;

private:
	// Clear the backdrop's internals
	virtual void					Undefine();

// Data members
private:
	RPsd3SingleGraphic*			m_pClockGraphic;
	RPsd3SingleGraphic*			m_pBigHandGraphic;
	RPsd3SingleGraphic*			m_pLittleHandGraphic;
	RRealRect						m_rClockRect;	
	RTime								m_rTime;

#ifdef	TPSDEBUG
// Debugging support
public:
	void Validate() const {}
#endif	// TPSDEBUG
};

#endif //_PSD3TIMEPIECEGRAPHIC_H_