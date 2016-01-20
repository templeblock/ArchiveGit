// ****************************************************************************
//
//  File Name:			TileAttributesExt.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				R. Grenfell
//
//  Description:		Declaration of the RTileAttributesExt class
//
//  Portability:		Platform independent
//
//  Developed by:		Brøderbund Software, Inc.
//
//	Copyright (C) 1997 Brøderbund Software, Inc., all rights reserved
//
// ****************************************************************************

#ifndef _TILEATTRIBUTESEXT_H_
#define _TILEATTRIBUTESEXT_H_

#include "ChunkyStorage.h"		// RChunkyStorage definition.

// written by Print Shop 6.0 (Via Leapfrog initially) and later
const YChunkTag	kExtendedTileAttributesChunkID = 0x03161001;

enum ETilingStyle
{
	TS_CENTERED = 0,	// Tiles with edges meeting in the center.
	TS_UPPERLEFT		// Tiles so that the upper left of the first tiled image
						// is always at the upper left of the bounding area.
};

class RTileAttributesExt : public RObject
{
	public:
		// Constructor.
		RTileAttributesExt()
			{ m_eTilingStyle = TS_CENTERED; }

		// Get and set methods.
		inline ETilingStyle	GetTilingStyle();
		inline void			SetTilingStyle( ETilingStyle eTilingStyle );

		// Serialization
		inline virtual void	Read( RChunkStorage& rStorage );
		inline virtual void	Write( RChunkStorage& rStorage ) const;

		// Assignment
		inline RTileAttributesExt& operator=( const RTileAttributesExt& rAttributes );

#ifdef TPSDEBUG
		inline virtual void	Validate( ) const;
#endif		

	private:
		// Specifies the tiling style.
		ETilingStyle m_eTilingStyle; 
};


// ****************************************************************************
// 					Inlines
// ****************************************************************************

ETilingStyle RTileAttributesExt::GetTilingStyle()
{
	return m_eTilingStyle;
}

void RTileAttributesExt::SetTilingStyle( ETilingStyle eTilingStyle )
{
	m_eTilingStyle = eTilingStyle;
}


void RTileAttributesExt::Read( RChunkStorage& rStorage )
{
	// Read the extended attributes, if any
	RChunkStorage::RChunkSearcher rSearcher = rStorage.Start(kExtendedTileAttributesChunkID, !kRecursive);
	if (!rSearcher.End())
	{
		uWORD uTilingStyle;
		rStorage >> uTilingStyle;
		m_eTilingStyle = (ETilingStyle)uTilingStyle;
#ifdef TPSDEBUG
		Validate();
#endif
		rStorage.SelectParentChunk();
	}
}

void RTileAttributesExt::Write( RChunkStorage& rStorage ) const
{
	rStorage.AddChunk(kExtendedTileAttributesChunkID);
	rStorage << (uWORD)m_eTilingStyle;
	rStorage.SelectParentChunk();
}

RTileAttributesExt& RTileAttributesExt::operator=( const RTileAttributesExt& rAttributes )
{
	m_eTilingStyle = rAttributes.m_eTilingStyle;
	return *this;
}

#ifdef TPSDEBUG
void RTileAttributesExt::Validate( ) const
{
	TpsAssert( this != NULL, "Invalid pointer to an extended tiling class object." );
	if (m_eTilingStyle != TS_CENTERED && m_eTilingStyle != TS_UPPERLEFT)
		TpsAssert( FALSE, "Invalid tiling style set" );
}
#endif

#endif //_TILEATTRIBUTESEXT_H_
