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

#ifndef _PSD3RULEDLINEGRAPHIC_H_
#define _PSD3RULEDLINEGRAPHIC_H_

#ifndef	_GRAPHIC_H_
#include	"Graphic.h"
#endif

#ifndef _PSD3GRAPHIC_H_
#include "Psd3Graphic.h"
#endif

#ifndef	_PSD3SINGLEGRAPHIC_H_
#include	"Psd3SingleGraphic.h"
#endif

typedef uLONG EPsd3RuledLineElement;

const YChunkTag kPsd3RuledLineGraphicChunkID = 0x02160005;

class RPsd3RuledLineGraphic : public RGraphic, public RPsd3Graphic
{
// Constants
public:
	// Constants (defined in constructor initializer list)
	const EPsd3RuledLineElement kPsd3RuledLineLeftCap;
	const EPsd3RuledLineElement kPsd3RuledLineRightCap;
	const EPsd3RuledLineElement kPsd3RuledLineRail;

// Member functions
public:
	// Construction/destruction
											RPsd3RuledLineGraphic();
	virtual								~RPsd3RuledLineGraphic();

	// Operations
	virtual BOOLEAN					Initialize(const uBYTE* pGraphicData, BOOLEAN fAdjustLineWidth = FALSE, BOOLEAN fMonochrome = FALSE);
	virtual BOOLEAN					ContainsGradientData();
	virtual BOOLEAN					GetAdjustLineWidth() {return TRUE;}
	virtual YChunkTag					GetChunkStorageTag() {return kPsd3RuledLineGraphicChunkID;}
	virtual void						Render(RDrawingSurface& rDrawingSurface, const RRealSize& rSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& monochromeColor = RSolidColor( kBlack ), BOOLEAN fImageAsMask = FALSE);
	virtual BOOLEAN					GetOutlinePath(RPath& rPath, const RRealSize& rSize, const R2dTransform& rTransform);
	virtual BOOLEAN					IsMonochrome() const { return GetMonochrome(); }

	void									SetHorizontal();
	void									SetVertical();

protected:
	// Clean up the internals
	virtual void						Undefine();
	
private:
	// Render helper functions
	void									Render(RDrawingSurface* pDS, RPath* pPath, const RRealSize& rSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor);
	void									RenderLeftCap(RDrawingSurface* pDS, RPath* pPath, const RRealSize& rLineSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor) const;
	void									RenderRightCap(RDrawingSurface* pDS, RPath* pPath, const RRealSize& rLineSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor) const;
	void									RenderRails(RDrawingSurface* pDS, RPath* pPath, const RRealSize& rLineSize, const R2dTransform& rTransform, const RIntRect& rRender, const RColor& rMonoColor) const;

// Data members
protected:
	// The pieces of the ruled line
	RPsd3SingleGraphic*				m_pLeftCap;
	RPsd3SingleGraphic*				m_pRightCap;
	RPsd3SingleGraphic*				m_pRail;
	BOOLEAN								m_fMirrorCaps;
	BOOLEAN								m_fHorizontal;	


#ifdef	TPSDEBUG
public:
	// Debugging support
	void Validate() const;
#endif	// TPSDEBUG
};

#endif //_PSD3RULEDLINEGRAPHIC_H_