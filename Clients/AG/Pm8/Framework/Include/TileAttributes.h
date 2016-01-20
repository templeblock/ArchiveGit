// ****************************************************************************
//
//  File Name:			TileAttributes.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RTileAttributes class
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
// ****************************************************************************

#ifndef _TILEATTRIBUTES_H_
#define _TILEATTRIBUTES_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

struct RTileAttributes
{	
	RRealSize			m_rTileSize;
	RRealSize			m_rTileSpacing;
};

// Operator<< (global)
inline RArchive& operator<<(RArchive& lhs, const RTileAttributes& rhs)
{
	lhs << rhs.m_rTileSize;
	lhs << rhs.m_rTileSpacing;
	
	return lhs;
}

// Operator>> (global)
inline RArchive& operator>>(RArchive& lhs, RTileAttributes& rhs)
{
	lhs >> rhs.m_rTileSize;
	lhs >> rhs.m_rTileSpacing;

	return lhs;
}


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_TILEATTRIBUTES_H_