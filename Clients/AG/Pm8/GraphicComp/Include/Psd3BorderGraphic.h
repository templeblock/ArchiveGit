// *****************************************************************************
//
// File name:		Psd3RuledLineGraphic.h
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

#ifndef _PSD3BORDERGRAPHIC_H_
#define _PSD3BORDERGRAPHIC_H_

#ifndef	_GRAPHIC_H_
#include	"Graphic.h"
#endif

#ifndef _PSD3GRAPHIC_H_
#include "Psd3Graphic.h"
#endif

#ifndef _PSD3SINGLEGRAPHIC_H_
#include "Psd3SingleGraphic.h"
#endif

// Define a default border thickness (RLUs)
const YRealDimension kDefaultBorderThickness = 200.0;

const YChunkTag kPsd3BorderGraphicChunkID = 0x02160006;

class RPsd3BorderGraphic : public RGraphic, public RPsd3Graphic
{
// Constants
public:
	enum EPsd3BorderElement
	{
		kPsd3None						= 0x0000,
		kPsd3BorderCorner				= 0x0001,
		kPsd3BorderTopConnector		= 0x0002,
		kPsd3BorderTopMiddle			= 0x0004,
		kPsd3BorderTopRail			= 0x0008,
		kPsd3BorderSideConnector	= 0x0010,
		kPsd3BorderSideMiddle		= 0x0020,
		kPsd3BorderSideRail			= 0x0040,
		kPsd3BorderTopAlt1			= 0x0080,
		kPsd3BorderTopAlt2			= 0x0100,
		kPsd3BorderSideAlt1			= 0x0200,
		kPsd3BorderSideAlt2			= 0x0400,
		kPsd3BorderYCornerFlip		= 0x0800,
		kPsd3BorderTopRailFlip		= 0x1000,
		kPsd3BorderSideRailFlip		= 0x2000
	};

// Member functions
public:
	// Construction/destruction
											RPsd3BorderGraphic();	
	virtual								~RPsd3BorderGraphic();

	// Operations
	virtual BOOLEAN					Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth, BOOLEAN fMonochrome);
	virtual void						Render(RDrawingSurface& rDrawingSurface, const RRealSize& rSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& monochromeColor = RSolidColor( kBlack ), BOOLEAN fImageAsMask = FALSE);
	virtual BOOLEAN					ContainsGradientData();
	virtual BOOLEAN					GetAdjustLineWidth() {return TRUE;}
	virtual YChunkTag					GetChunkStorageTag() {return kPsd3BorderGraphicChunkID;}
	virtual BOOLEAN					GetOutlinePath(RPath& path, const RRealSize& size, const R2dTransform& transform);
	virtual BOOLEAN					IsMonochrome() const { return GetMonochrome(); }
	
	void									SetBorderThickness(YRealDimension yThickness);
	YRealDimension						GetBorderThickness() const;	
	virtual void						OnXDisplayChange( );

protected:
	virtual void						Undefine();

private:	
	void									Render(RDrawingSurface* pDrawingSurface, RPath* pPath, const RRealSize& rBorderSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor);
	void									RenderHorizontal(RDrawingSurface* pDrawingSurface, RPath* pPath, const RRealSize& rHorizSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor );
	void									RenderVertical(RDrawingSurface* pDrawingSurface, RPath* pPath, const RRealSize& rVertSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor );
	void									RenderCorners(RGraphic* pGraphic, const RRealSize& rGraphicSize, BOOLEAN fMirror, BOOLEAN fInvert, RDrawingSurface* pDrawingSurface, RPath* pPath, const RRealSize& rBorderSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor);
	void									UpdateX(const RRealSize& rGraphicSize, RRealSize& rBoxSize, R2dTransform& rTransform);
	void									UpdateY(const RRealSize& rGraphicSize, RRealSize& rBoxSize, R2dTransform& rTransform);

	static void							InitializeRenderCache(RPsd3SingleGraphic* pGraphic) { if (pGraphic) { pGraphic->GetRenderCache().Enable(TRUE); pGraphic->GetRenderCache().SetRenderThreshold(0); } }
	static void							CallDisplayChange( RPsd3SingleGraphic* pGraphic ) { if (pGraphic) pGraphic->OnXDisplayChange( ); }

// Data members
protected:	
	// The border graphic elements
	RPsd3SingleGraphic*				m_pCorner;
	RPsd3SingleGraphic*				m_pHorizontalConnector;
	RPsd3SingleGraphic*				m_pHorizontalMiddle;
	RPsd3SingleGraphic*				m_pHorizontalRail;
	RPsd3SingleGraphic*				m_pHorizontalAlt1;
	RPsd3SingleGraphic*				m_pHorizontalAlt2;	
	RPsd3SingleGraphic*				m_pVerticalConnector;
	RPsd3SingleGraphic*				m_pVerticalMiddle;
	RPsd3SingleGraphic*				m_pVerticalRail;
	RPsd3SingleGraphic*				m_pVerticalAlt1;
	RPsd3SingleGraphic*				m_pVerticalAlt2;

	// The thickness of the border
	YRealDimension						m_yBorderThickness;	
	
	BOOLEAN								m_fMirrorCorner;
	BOOLEAN								m_fInvertCorner;	

	BOOLEAN								m_fInvertHorizontalMiddle;
	BOOLEAN								m_fInvertVerticalMiddle;

	BOOLEAN								m_fMirrorHorizontalConnector;
	BOOLEAN								m_fMirrorHorizontalRail;
	BOOLEAN								m_fMirrorHorizontalAlt1;
	BOOLEAN								m_fMirrorHorizontalAlt2;
	BOOLEAN								m_fMirrorVerticalConnector;
	BOOLEAN								m_fMirrorVerticalRail;
	BOOLEAN								m_fMirrorVerticalAlt1;
	BOOLEAN								m_fMirrorVerticalAlt2;

	BOOLEAN								m_fInvertHorizontalConnector;
	BOOLEAN								m_fInvertHorizontalRail;
	BOOLEAN								m_fInvertHorizontalAlt1;
	BOOLEAN								m_fInvertHorizontalAlt2;
	BOOLEAN								m_fInvertVerticalConnector;
	BOOLEAN								m_fInvertVerticalRail;
	BOOLEAN								m_fInvertVerticalAlt1;
	BOOLEAN								m_fInvertVerticalAlt2;

	BOOLEAN								m_fStretchHorizontalRail;
	BOOLEAN								m_fStretchVerticalRail;

#ifdef	TPSDEBUG
public:
	// Debugging support
	void Validate( ) const;
#endif	// TPSDEBUG
};

#endif //_PSD3BORDERGRAPHIC_H_