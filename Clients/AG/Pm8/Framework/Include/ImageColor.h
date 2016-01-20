//****************************************************************************
//
// File Name:   ImageColor.h
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Definition of the IImageColorData, RImageColorData,
//              and RImageColor objects
//
// Portability: Platform Independent
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/ImageColor.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _IMAGECOLOR_H_
#define _IMAGECOLOR_H_

#include "Interface.h"
#include "ThreadSupport.h"

#ifndef	_MEMORYMAPPEDBUFFER_H_
#include	"MemoryMappedBuffer.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RDrawingSurface;
class RBitmapImage;
class RImage;


const YInterfaceId kImageColorDataInterface = 103;

///////////////////////////////////////////////////////////////////////////////
// IImageColorData Declaration
///////////////////////////////////////////////////////////////////////////////

class IImageColorData : public RIUnknown
{
public:

	virtual void			Initialize( const RBitmapImage* pImage ) = 0;
	virtual void			Initialize( RBuffer& rBuffer ) = 0;

	virtual void			Read( RArchive& archive ) = 0;
	virtual void			Write( RArchive& archive ) const = 0;
	virtual void			Render( RDrawingSurface& ds, const RIntRect& rcBounds, const R2dTransform& transform ) = 0;

	virtual uLONG			GetDataLength() const = 0;
	virtual RBitmapImage*	GetImage( YTint tint = kMaxTint ) = 0;
	virtual RRealSize		GetSizeInLogicalUnits() = 0;
};


///////////////////////////////////////////////////////////////////////////////
// RImageColorData Declaration
///////////////////////////////////////////////////////////////////////////////

class RImageColorData : public IImageColorData
{
public:
	
							RImageColorData();
	virtual					~RImageColorData();

								// RIUnknown overrides
	virtual uLONG			AddRef( void ); 
	virtual uLONG			Release( void ); 
	virtual BOOLEAN			QueryInterface( YInterfaceId, void ** ppvObject );

							// IImageColorData overrides
	virtual void			Initialize( const RBitmapImage* pImage );
	virtual void			Initialize( RBuffer& rBuffer );
	virtual void			Read( RArchive& archive );
	virtual void			Write( RArchive& archive ) const;

	virtual void			Render( RDrawingSurface& ds, const RIntRect& rcBounds, const R2dTransform& transform );

	virtual uLONG			GetDataLength() const;
	virtual RRealSize		GetSizeInLogicalUnits();
	virtual RBitmapImage*	GetImage( YTint tint = kMaxTint );

								// Class factory
	static BOOLEAN			CreateInstance( YInterfaceId, void ** ppvObject );

protected:

	void						CompressInternalData();

private:

	RCriticalSection		m_CriticalSection;

	RTempFileBuffer		m_rInternalDataBuffer;
	RBitmapImage*			m_pImage ;
	uLONG						m_nRefCount;

	YTint						m_nLastTint;
};


///////////////////////////////////////////////////////////////////////////////
// RImageColor Declaration
///////////////////////////////////////////////////////////////////////////////

class FrameworkLinkage RImageColor 
{
public:

								enum 
								{ 
									kCentered    = 0x0001,
									kTiled       = 0x0002,
									kPhoto		 = 0x0004,
								};

								RImageColor( uWORD uwFlags = kCentered | kTiled );
								RImageColor( uLONG ulID, const RImage* pImage );
								RImageColor( const RImageColor& rhs );
								~RImageColor();

	BOOLEAN					Initialize( uLONG ulID, RBuffer& rBuffer );
	BOOLEAN					Initialize( const RBitmapImage* pImage );
	BOOLEAN					Initialize( RMBCString rPath );

	const RImageColor&	operator=( const RImageColor& rhs );
	BOOLEAN					operator==( const RImageColor& rhs ) const;
	BOOLEAN					operator!=( const RImageColor& rhs ) const;

	void						Read( RArchive& archive );
	void						Write( RArchive& archive ) const;

	void						ApplyTransform( const R2dTransform& transform );
	const R2dTransform&	GetBaseTransform( );
	void						SetBaseTransform( const R2dTransform& basetransform );
	void						SetBoundingRect( const RIntRect& rcBounds );

	uLONG						GetDataLength();

	uWORD						GetFlags() const;
	void						SetFlags( uWORD wFlags );

	YTint						GetTint( ) const;
	void						SetTint( YTint tint );

	const RBitmapImage*	GetImage();

	void						Fill( RDrawingSurface& ds, const RIntRect& rcBounds, R2dTransform transform ) const;

	friend class RColor;

protected:

	RBitmapImage*			GetProxy( RDrawingSurface& ds, YAngle angle, YRealDimension xScale, YRealDimension yScale );
	void						Render( RDrawingSurface& ds, const RIntRect& rcBounds, R2dTransform transform ) const;
	void						RenderTiled( RDrawingSurface& ds, const RIntRect& rcBounds, R2dTransform transform ) const;

private:

	IImageColorData*		m_pImageData;

								// Serialized data
	uLONG						m_ulID;
	uWORD						m_uwIndex;
	uWORD						m_uwFlags;
	YTint						m_nTint;
	R2dTransform				m_BaseTransform;

								// Non-Serialized data
	RIntRect					m_rcBounds;
	R2dTransform				m_Transform;

	RIntSize					m_ptXOffset;
	RIntSize					m_ptYOffset;

	RBitmapImage*				m_pProxyImage;
	RBitmapImage*				m_pProxyMask;
	YAngle						m_nLastAngle;

	friend class RTextureGridCtrl;
};

//****************************************************************************
//
// RImageColor::GetBaseTransform()
//
// @MFunc Returns the base transformation
//
// @RDesc ^
//
inline const R2dTransform& RImageColor::GetBaseTransform()
{
	return m_BaseTransform;
}

//****************************************************************************
//
// void RImageColor::SetBaseTransform( const R2dTransform& basetransform )
//
// @MFunc Set the image color's base transformation matrix
//
// @RDesc nothing
//
inline void RImageColor::SetBaseTransform( const R2dTransform& basetransform )
{
	m_BaseTransform	= basetransform;
}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _IMAGECOLOR_H_
