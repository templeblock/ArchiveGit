// ****************************************************************************
//
//  File Name:			PixelGrid.h
//
//  Project:			TPS Libraries
//
//  Author:				Greg Brown, Bruce Rosenblum
//
//  Description:		Advanced DIB manipulation functions support routines
//							Maintains a bitmap "hit-table" used to indicate which
//							pixels in an image have been accessed
//
//  Portability:		Platform independent, 32 bit systems only
//
//  Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Turning Point Software         
//
//  Copyright (C) 1996 Turning Point Software, Inc. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/PixelGrid.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _PIXELGRID_H_
#define _PIXELGRID_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

const uBYTE BITMASK[8] = {128, 64, 32, 16, 8, 4, 2, 1};
const uBYTE LONGMASK = 0x00000007;

class CPixelGrid
{
public:
										CPixelGrid ();
										~CPixelGrid();
	void								Initialize(sLONG Width, sLONG Height);
	void								MarkPoint (const RIntPoint& Point);
	BOOLEAN							IsPointDirty (const RIntPoint& Point);

private:
	PUBYTE*							m_ppPixelGrid;		
	sLONG								m_Width;
	sLONG								m_Height;
};

// ****************************************************************************
//
//  Function Name:	CPixelGrid::MarkPoint( )
//
//  Description:		Mark a point in the pixel array to flag it has been used
//
//  Returns:			Nothing
//
//  Exceptions:		none
//
// ****************************************************************************
//
inline void CPixelGrid::MarkPoint (const RIntPoint& point)
{
	TpsAssert (NULL != m_ppPixelGrid, "Bad grid pointer");
	TpsAssert (point.m_x >= 0 && point.m_x < m_Width, "Bad point x value");
	TpsAssert (point.m_y >= 0 && point.m_y < m_Height, "Bad point y value");

	int	Column = point.m_x >> 3;
	int	BitIndex = point.m_x & LONGMASK;
	m_ppPixelGrid[point.m_y][Column] = static_cast<uBYTE>
		((m_ppPixelGrid[point.m_y][Column] & (~BITMASK[BitIndex])) | BITMASK[BitIndex]);
}

// ****************************************************************************
//
//  Function Name:	CPixelGrid::IsPointDirty( )
//
//  Description:		Return flag from a point in the pixel array to determine used
//
//  Returns:			Boolean TRUE if used, FALSE if not used
//
//  Exceptions:		none
//
// ****************************************************************************
//
inline BOOLEAN CPixelGrid::IsPointDirty (const RIntPoint& point)
{
	TpsAssert (NULL != m_ppPixelGrid, "Bad grid pointer");
	TpsAssert (point.m_x >= 0 && point.m_x < m_Width, "Bad point x value");
	TpsAssert (point.m_y >= 0 && point.m_y < m_Height, "Bad point y value");

	return (BOOLEAN)((m_ppPixelGrid[point.m_y][point.m_x >> 3] & BITMASK[point.m_x & LONGMASK]) != 0);
}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _PIXELGRID_H_




	

