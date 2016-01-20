// ****************************************************************************
//
//  File Name:			SingleGraphic.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RSingleGraphic class
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
// ****************************************************************************

#ifndef		_SINGLEGRAPHIC_H_
#define		_SINGLEGRAPHIC_H_

#include		"Graphic.h"
#include		"GradientFill.h"
#include		"Path.h"
#include		"RenderCache.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

enum EOperation {	kSetLineWidth,
						kSetLineStyle,
						kSetFillColor,
						kSetLineColor,
						kPolyline,
						kPolygon,
						kPath,
						kEndGraphic,
						kSetGradient,
						kEllipse };

typedef int YPathOperatorArraySize;
typedef uBYTE YPathOperatorRepCount;

class RStorage;
class RChunkStorage;
class RBitmapImage;
class RSingleGraphicRenderCache;

const int kMaxPathPolygons = 512;

const YFloatType kMaxGraphicError = 0.20;

// local array and iterator defs
typedef	RArray<RColor>									RColorArray;
typedef	RColorArray::YIterator						RColorArrayIterator;
typedef	RArray<EOperation>							EOperationArray;
typedef	EOperationArray::YIterator					EOperationArrayIterator;
typedef	RArray<YPenWidth>								YPenWidthArray;
typedef	YPenWidthArray::YIterator					YPenWidthArrayIterator;
typedef	RArray<YPenStyle>								YPenStyleArray;
typedef	YPenStyleArray::YIterator					YPenStyleArrayIterator;
typedef	RArray<YFillMethod>							YFillMethodArray;
typedef	YFillMethodArray::YIterator				YFillMethodArrayIterator;
typedef	RArray<YPointCount>							YPointCountArray;
typedef	YPointCountArray::YIterator				YPointCountArrayIterator;
typedef	RArray<RGradientFill>						YGradientFillArray;
typedef	YGradientFillArray::YIterator				YGradientFillArrayIterator;
typedef	RArray<RGradientRamp>						YGradientRampArray;
typedef	YGradientRampArray::YIterator				YGradientRampArrayIterator;
typedef	YCounter											YGradientIndex;
typedef	RArray<YGradientIndex>						YGradientIndexArray;
typedef	YGradientIndexArray::YIterator			YGradientIndexArrayIterator;

// Chunk IDs
// 0x00762400+
const YChunkTag kSingleGraphicOperationsStream = 			0x00762401;
const YChunkTag kSingleGraphicColorsStream =					0x00762402;
const YChunkTag kSingleGraphicPenWidthsStream =				0x00762403;
const YChunkTag kSingleGraphicPenStylesStream =				0x00762404;
const YChunkTag kSingleGraphicFillMethodsStream =			0x00762405;
const YChunkTag kSingleGraphicPointCountsStream =			0x00762406;
const YChunkTag kSingleGraphicPathOperatorsStream =		0x00762407;
const YChunkTag kSingleGraphicOriginalPointsStream =		0x00762408;
const YChunkTag kSingleGraphicTransformedPointsStream =	0x00762409;
const YChunkTag kSingleGraphicGradientFillStream =			0x0076240A;
const YChunkTag kSingleGraphicGradientRampStream =			0x0076240B;
const YChunkTag kSingleGraphicGradientIndexStream =		0x0076240C;
const YChunkTag kSingleGraphicIncidentalData =				0x0076240D;

// ****************************************************************************
//
// Class Name:			RSingleGraphic
//
// Description:		Base class for Single Graphics, headlines, etc.
//
// ****************************************************************************
//
class FrameworkLinkage RSingleGraphic : public RGraphic
	{
	//
	// Allow the RPath to create our internal data directly...
	friend class RPath;
	//
	// Allow headline graphics to create out internal data directly...
	friend class RHeadlineGraphic;

	// Construction, Destruction & Initialization
	public :
												RSingleGraphic( BOOLEAN fCreateRender = TRUE );
												RSingleGraphic( const RSingleGraphic &rhs );
		virtual								~RSingleGraphic( );

	// Operations
	public :
		RSingleGraphic&					operator=( const RSingleGraphic& rhs );

		virtual void						Render( RDrawingSurface& drawingSurface,
														  const RRealSize& size,
														  const R2dTransform& transform,
														  const RIntRect& rRender,
														  const RColor& monochromeColor = RSolidColor( kBlack ),
														  BOOLEAN fImageAsMask = FALSE );

		void									MergeWith( const R2dTransform& transform,
															  RSingleGraphic& graphic );

		void									AppendGraphic( RSingleGraphic& graphic,
															const R2dTransform& transform );

		virtual BOOLEAN					GetOutlinePath(RPath& path, const RRealSize& size, const R2dTransform& transform);
		
		RIntRect								GetFirstBox();

		virtual void						OnXDisplayChange( );

	// Implementation
	private :
		void									DoRender( RDrawingSurface& drawingSurface,
														    const RRealSize& size,
														    const R2dTransform& transform,
														    const RColor& monochromeColor,
														    BOOLEAN fImageAsMask );		

	// Operations
	public :
		void									Undefine( );
		void									Undefine( const RSolidColor& stroke, const RSolidColor& fill );
		void									SetDefaultColors( const RSolidColor& stroke, const RColor& fill );
		void									SetFillMethods( YFillMethod method );
		void									SetFillMethods( YCounter nrMethods, YFillMethod method );
		void									SetDefaultPenWidth( YPenWidth width );
		
		RRenderCache&						GetRenderCache();

	// Accessors
	public :
		virtual YFloatType				GetAspectRatio() const;
		virtual RRealSize					GetGraphicSize() const;		
		BOOLEAN								IsMonochrome( ) const;

	// Implementation
	protected :
		void									DoApplyTransform( const R2dTransform& transform, BOOLEAN fIntegerAlso );
		void									ApplyTransform( const R2dTransform& transform );

		YIntDimension						CalcPenWidth( YPenWidth graphicPenWidth,
																  const RRealSize& outputLogicalSize,
																  const R2dTransform& renderTransform ) const;

		R2dTransform						CalcRenderXform( const RRealSize& size, const R2dTransform& transform ) const;

	// Member data
	protected :
		EOperationArray					m_OperationsStream;
		RColorArray							m_ColorsStream;
		YPenWidthArray						m_PenWidthsStream;
		YPenStyleArray						m_PenStylesStream;
		YFillMethodArray					m_FillMethodsStream;
		YPointCountArray					m_PointCountsStream;
		EPathOperatorArray				m_PathOperatorsStream;
		RRealPointArray					m_OriginalPointsStream;
		RIntPointArray						m_TransformedIntPointsStream;
		RRealPointArray					m_TransformedRealPointsStream;
		YGradientFillArray				m_GradientFillStream;
		YGradientRampArray				m_GradientRampStream;
		YGradientIndexArray				m_GradientIndexStream;

		RRealSize							m_GraphicSize;
		BOOLEAN								m_fMonochrome;
		BOOLEAN								m_fAdjustLineWidth;
		R2dTransform						m_LastTransform;
		BOOLEAN								m_fFirstBox;
		RRealRect							m_rFirstBox;
		RSingleGraphicRenderCache*		m_pRenderCache;
		BOOLEAN								m_fColorTransformInvalid;

	friend class RSingleGraphicRenderCache;

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
		};

// ****************************************************************************
//
// Class Name:			RSingleGraphicRenderCache
//
// Description:		Render cache for single graphics
//
// ****************************************************************************
//
class RSingleGraphicRenderCache : public RRenderCache
	{
	// Construction
	public :
												RSingleGraphicRenderCache( RSingleGraphic* pGraphic );

	// Operations
	public :
		void									RenderGraphic( RDrawingSurface& drawingSurface,
																	const RRealSize& size,
																	const R2dTransform& transform,
																	const RIntRect& rcRender,
																	const RColor& monochromeColor );

	// Implementation
	private :
		void									RenderData( RDrawingSurface& drawingSurface,
																const RRealSize& size,
																const R2dTransform& transform,
																const RIntRect& rcRender ) const;

	// Member data
	private :
		RSingleGraphic*					m_pGraphic;
		RColor								m_MonochromeColor;
	};

//
// MGD: This class is used purely for creating temporary graphics to be created
// for merging purposes...
class RTempSingleGraphic : public RSingleGraphic
{
	public :
													RTempSingleGraphic( ) : RSingleGraphic( FALSE ) {;}
	public:
		virtual void							Read( RChunkStorage& rStorage );
		virtual void							Write( RChunkStorage& rStorage ) const;
};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
// Function Name:		RSingleGraphic::IsMonochrome( )
//
// Description:		Accessor
//
// Returns:				TRUE if the graphic is a monochrome graphic, FALSE
//							otherwise.
//
// Exceptions:			None
//
// ****************************************************************************
//
inline BOOLEAN RSingleGraphic::IsMonochrome( ) const
	{
	return m_fMonochrome;
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::GetGraphicSize( )
//
// Description:		
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RRealSize RSingleGraphic::GetGraphicSize( ) const
	{
	return m_GraphicSize;
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::GetAspectRatio( )
//
// Description:		Returns the aspect ratio of the graphic
//
// Returns:				
//
// Exceptions:			
//
// ****************************************************************************
inline YFloatType RSingleGraphic::GetAspectRatio() const
	{
	return GetGraphicSize().m_dx / GetGraphicSize().m_dy;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _SINGLEGRAPHIC_H_
