// *****************************************************************************
//
// File name:			Psd3BannerGraphic.h
//
// Project:				Renaissance Framework
//
// Author:				G. Brown
//
// Description:		Declaration of the RPsd3HorizontalBannerGraphic and 
//							RPsd3VerticalBannerGraphic classes.
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

#ifndef _PSD3BANNERGRAPHIC_H_
#define _PSD3BANNERGRAPHIC_H_

#ifndef _GRAPHIC_H_
#include "Graphic.h"
#endif

#ifndef _PSD3GRAPHIC_H_
#include "Psd3Graphic.h"
#endif

#ifndef _PSD3SINGLEGRAPHIC_H_
#include "Psd3SingleGraphic.h"
#endif

const YChunkTag kPsd3HorizontalBannerGraphicChunkID =	0x02160003;
const YChunkTag kPsd3VerticalBannerGraphicChunkID =		0x02160004;

// *****************************************************************************
//		Class name:		RBannerGraphic
// *****************************************************************************
class RBannerGraphic : public RGraphic
{
// Member functions
public:
	// Operations
	virtual void						Render( RDrawingSurface& rDrawingSurface, const RRealSize& rSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& monochromeColor = RSolidColor( kBlack ), BOOLEAN fImageAsMask = FALSE) = 0;
	virtual BOOLEAN					GetOutlinePath(RPath& rPath, const RRealSize& rSize, const R2dTransform& rTransform) = 0;	

protected:
	// Clear the graphic's internals
	virtual void						Undefine() = 0;
};

// *****************************************************************************
//		Class name:		RPsd3HorizontalBannerGraphic
// *****************************************************************************
class RPsd3HorizontalBannerGraphic : public RBannerGraphic, public RPsd3Graphic
{
// Constants
public:
	enum EBannerBackdropElement
	{
		kLeftCap = 1,
		kRightCap = 2,
		kTopRail = 4,
		kBottomRail = 8
	};

// Member functions
public:
	// Construction/destruction
											RPsd3HorizontalBannerGraphic();
	virtual								~RPsd3HorizontalBannerGraphic();

	// Operations	
	virtual BOOLEAN					Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth = FALSE, BOOLEAN fMonochrome = FALSE);
	virtual BOOLEAN					ContainsGradientData();
	virtual BOOLEAN					GetAdjustLineWidth() {return FALSE;}
	virtual YChunkTag					GetChunkStorageTag() {return kPsd3HorizontalBannerGraphicChunkID;}
	virtual void						Render( RDrawingSurface& rDrawingSurface, const RRealSize& rSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& monochromeColor = RSolidColor( kBlack ), BOOLEAN fImageAsMask = FALSE);
	virtual BOOLEAN					GetOutlinePath(RPath& rPath, const RRealSize& rSize, const R2dTransform& rTransform);
	virtual BOOLEAN					IsMonochrome() const { return GetMonochrome(); }

protected:
	// Clear the backdrop's internals
	virtual void						Undefine();

// Data members
protected:
	// Horizontal banner graphics
	RPsd3SingleGraphic*				m_pLeftCap;	
	RPsd3SingleGraphic*				m_pRightCap;
	RPsd3SingleGraphic*				m_pTopRail;
	RPsd3SingleGraphic*				m_pBottomRail;	

	// Mirror the endcaps?
	BOOLEAN								m_fMirrorCaps;	
};

// *****************************************************************************
//		Class name:		RPsd3VerticalBannerGraphic
// *****************************************************************************
class RPsd3VerticalBannerGraphic : public RBannerGraphic, public RPsd3Graphic
{
// Constants
public:
	enum EBannerElement
	{
		kTopCap = 1,
		kBottomCap = 2,
		kLeftRail = 4,
		kRightRail = 8
	};

// Member functions
public:
	// Construction/destruction
											RPsd3VerticalBannerGraphic();
	virtual								~RPsd3VerticalBannerGraphic();

	// Operations
	virtual BOOLEAN					Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth = FALSE, BOOLEAN fMonochrome = FALSE);
	virtual BOOLEAN					ContainsGradientData();
	virtual BOOLEAN					GetAdjustLineWidth() {return FALSE;}
	virtual YChunkTag					GetChunkStorageTag() {return kPsd3VerticalBannerGraphicChunkID;}
	virtual void						Render( RDrawingSurface& rDrawingSurface, const RRealSize& rSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& monochromeColor = RSolidColor( kBlack ), BOOLEAN fImageAsMask = FALSE);
	virtual BOOLEAN					GetOutlinePath(RPath& rPath, const RRealSize& rSize, const R2dTransform& rTransform);
	virtual BOOLEAN					IsMonochrome() const { return GetMonochrome(); }

protected:
	// Clear the backdrop's internals
	virtual void						Undefine();

// Data members
protected:
	// Vertical banner graphics
	RPsd3SingleGraphic*				m_pTopCap;	
	RPsd3SingleGraphic*				m_pBottomCap;
	RPsd3SingleGraphic*				m_pLeftRail;
	RPsd3SingleGraphic*				m_pRightRail;	

	// Mirror the endcaps?
	BOOLEAN								m_fMirrorCaps;	
};

#endif // _PSD3BANNERGRAPHIC_H_