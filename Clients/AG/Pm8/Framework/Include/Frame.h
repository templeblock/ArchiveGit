// ****************************************************************************
//
//  File Name:			Frame.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				Eric VanHelene
//
//  Description:		Declaration of the RFrame class
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
//  $Logfile:: /PM8/Framework/Include/Frame.h                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FRAME_H_
#define		_FRAME_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RDrawingSurface;
class RComponentView;

// ****************************************************************************
//
//  Class Name:		RFrameTypeMapEntry
//
//  Description:		Class that maps a command id to a zoom level
//
// ****************************************************************************
//
class FrameworkLinkage RFrameTypeMapEntry
	{
	public :
												RFrameTypeMapEntry( YCommandID commandId, EFrameType frameType );
		static YCommandID					GetCommandId( const EFrameType eType );
		static EFrameType					GetFrameType( const YCommandID yID );

		YCommandID							m_CommandId;
		EFrameType							m_eFrameType;
	};

class YFrameTypeMap : public RArray<RFrameTypeMapEntry>{ };
typedef YFrameTypeMap::YIterator YFrameTypeIterator;

// ****************************************************************************
//
//  Class Name:		RFrame
//
//  Description:		Base class for Frames.
//
// ****************************************************************************
//
class FrameworkLinkage RFrame : public RObject
	{
	// enums
	public :
		enum									EFrameDrawType	{		kFrameDrawOutside=1, kFrameFillInside=2, kFrameAndFill=3	};

	// Construction & Destruction
	protected :
												RFrame( );
	//	
	public :
		virtual								~RFrame( );
		static RFrame*						GetFrame( const EFrameType eFrame, RComponentView* pView );

		EFrameType							GetFrameType( ) const;

	//	Operations
	public :
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform,
													const RIntRect& rcRender, const RSolidColor& rFrameColor, RColor& rBackgroundColor,
													const EFrameDrawType eDrawType = kFrameAndFill ) const;
		void									GetInset( RRealSize& topleft, RRealSize& bottomright ) const;
		void									GetInsetComponentRect( YComponentBoundingRect& boundingRect );

	protected :
		virtual YRealDimension			GetDeviceUnit( const R2dTransform& transform, const YRealDimension& ydim ) const;
		virtual void						DrawSquareEndLine( RDrawingSurface& drawingSurface, const RRealPoint& rTopLeft, const RRealPoint& rBottomRight, const R2dTransform& transform ) const;
		virtual void						DrawSquareEndRect( RDrawingSurface& drawingSurface, const RRealRect& rRect, const R2dTransform& transform ) const;

#ifdef	TPSDEBUG								
	// Debugging stuff						
	public :										
		virtual void						Validate( ) const;
#endif

	//	data members
	protected:
		EFrameType							m_eFrameType;
		RRealSize							m_TopLeftInset;
		RRealSize							m_BottomRightInset;
	};												

// ****************************************************************************
//
//  Class Name:		RSingleLineFrame
//
//  Description:		Base Class for Single Line Frames.
//
// ****************************************************************************
//
class FrameworkLinkage RSingleLineFrame	: public RFrame
	{
	//	Operations
	protected :

		//	Common code for single line frames.
		void							DrawSingleLineFrame( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
													const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType = kFrameAndFill ) const;
	};

// ****************************************************************************
//
//  Class Name:		RThinLineFrame
//
//  Description:		Class for Thin Line Frames.
//
// ****************************************************************************
//
class FrameworkLinkage RThinLineFrame : public RSingleLineFrame
	{
	// Construction
	private :																					
												RThinLineFrame( );

	public :
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
															const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType = kFrameAndFill ) const;
	friend class RFrame;
	};												

// ****************************************************************************
//
//  Class Name:		RMediumLineFrame
//
//  Description:		Class for Meduim Line Frames.
//
// ****************************************************************************
//
class FrameworkLinkage RMediumLineFrame : public RSingleLineFrame
	{
	// Construction
	private :
												RMediumLineFrame( );
	//	Operations
	public :
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
															const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType = kFrameAndFill ) const;
	friend class RFrame;
	};												

// ****************************************************************************
//
//  Class Name:		RThickLineFrame
//
//  Description:		Class for Thick Line Frames.
//
// ****************************************************************************
//
class FrameworkLinkage RThickLineFrame : public RSingleLineFrame
	{
	// Construction
	private :
												RThickLineFrame( );
	//
	//	Operations
	public :
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender
												, const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType = kFrameAndFill ) const;
	friend class RFrame;
	};												

// ****************************************************************************
//
//  Class Name:		RSingleLineFrame
//
//  Description:		Base Class for Single Line Frames.
//
// ****************************************************************************
//
class FrameworkLinkage RDoubleLineFrame	: public RFrame
	{
	//	Construction
	protected :
												RDoubleLineFrame( ) : RFrame( ) { m_LineToGapRatio = 1.0; }
	//	Operations
	protected :
		//	Common code for double line frames.
		virtual void						DrawDoubleLineFrame( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
													const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType = kFrameAndFill ) const;

	//	Members
	protected :
		YRealDimension						m_LineToGapRatio;
	};

// ****************************************************************************
//
//  Class Name:		RDoubleLineFrame
//
//  Description:		Class for Double Line Frames.
//
// ****************************************************************************
//
class FrameworkLinkage RDoubleThinLineFrame : public RDoubleLineFrame
	{
	// Construction
	private :
												RDoubleThinLineFrame( );
	//
	//	Operations
	public :
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
															const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType = kFrameAndFill ) const;
	friend class RFrame;
	};												

// ****************************************************************************
//
//  Class Name:		RDoubleMediumLineFrame
//
//  Description:		Class for Double Medium Line Frames.
//
// ****************************************************************************
//
class FrameworkLinkage RDoubleMediumLineFrame : public RDoubleLineFrame
	{
	//	Construction
	private :																					
												RDoubleMediumLineFrame( );
	//
	//	Operations
	public :
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
															const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType = kFrameAndFill ) const;
	friend class RFrame;
	};												

// ****************************************************************************
//
//  Class Name:		RDoubleThickLineFrame
//
//  Description:		Class for Double Thick Line Frames.
//
// ****************************************************************************
//
class FrameworkLinkage RDoubleThickLineFrame : public RDoubleLineFrame
	{
	// Construction
	private :																					
												RDoubleThickLineFrame( );
	//	Operations
	public :
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
													const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType = kFrameAndFill ) const;
	friend class RFrame;
	};												

// ****************************************************************************
//
//  Class Name:		RRoundCornerFrame
//
//  Description:		Class for Round Corner Frames.
//
// ****************************************************************************
//
class FrameworkLinkage RRoundCornerFrame : public RFrame
	{
	// Construction
	private :
												RRoundCornerFrame( );
	//
	//	Operations
	public :
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
															const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType = kFrameAndFill ) const;
	//	Members
	private :
		YRealDimension						m_WidthToRadiusRatio;

	friend class RFrame;
	};												

// ****************************************************************************
//
//  Class Name:		RDropShadowFrame
//
//  Description:		Class for Drop Shadow Frames.
//
// ****************************************************************************
//
class FrameworkLinkage RDropShadowFrame : public RFrame
	{
	//	Construction
	private :
												RDropShadowFrame( );
	//
	//	Operations
	public :
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
															const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType = kFrameAndFill ) const;
	friend class RFrame;
	};												

// ****************************************************************************
//
//  Class Name:		RPictureFrame
//
//  Description:		Class for Picture Frames.
//
// ****************************************************************************
//
class FrameworkLinkage RPictureFrame : public RFrame
	{
	// Construction
	private :
												RPictureFrame( );
	//
	// Operations
	public :
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
															const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType = kFrameAndFill ) const;
	friend class RFrame;
	};												

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		//	_FRAME_H_
