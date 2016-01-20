//////////////////////////////////////////////////////////////////////////////
//
// $Header: /PM8/App/BmpTiler.h 1     3/03/99 6:03p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/BmpTiler.h $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 4     2/17/99 10:12a Mwilson
// modified tiling
// 
// 3     2/16/99 5:29p Mwilson
// added tiler that handles RDrawingSurface and RBitmapImage
// 
// 2     3/06/98 1:07p Hforman
// 
// 1     3/06/98 11:02a Hforman
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __BMPTILER_H__
#define __BMPTILER_H__

////////////////////////////////////////////////////////////////////////////
// CBmpTiler -- helper class that tiles a bitmap onto a larger area. Also
// tiles borders around the edges

//
// The border boundaries are defined by 4 pts:
//
//
//            vert left      vert right
//                |              |
//              __V______________V__
//             |                    |
// horz upper->|   ______________   |
//             |  |              |  |
//             |  |              |  |
//             |  |              |  |
//             |  |              |  |
//             |  |              |  |
//             |  |              |  |
// horz lower->|  |______________|  |
//             |                    |
//             |____________________|
//
struct BorderPts
{
	int nHorzUpper;	// y pos of upper boundary
	int nHorzLower;	// y pos of lower boundary
	int nVertLeft;		// x pos of left boundary
	int nVertRight;	// x pos of right boundary
};

class CBmpTiler
{
public:
	CBmpTiler();
	~CBmpTiler();

	void TileBmpInRect(RDrawingSurface& drawingSurface, const CRect& rcBkgndPortion, RBitmapImage* pImage);
	void TileBmpInRect(CDC& dc, const CRect& rcArea, CBitmap& bitmap);
	void TileBmpAndBorderInRect(CDC& dc, const CRect& rcArea, CBitmap& bmTile,
										 CBitmap& bmBorder,
										 BorderPts pts);

protected:
	void DoTile(RDrawingSurface& drawingSurface, const CRect& rcBkgndPortion, RBitmapImage* pImage,
					BOOL fHasBorder=FALSE, RBitmapImage* pBorderImage=NULL, BorderPts* pts=NULL);

	void DoTile(CDC& dc, const CRect& rcArea, CBitmap& bmTile,
					BOOL fHasBorder=FALSE, CBitmap* pbmBorder=NULL, BorderPts* pts=NULL);

	void TileBmpIntoMemDC(CDC& memDC, CDC& bmDC, CRect rcArea, CBitmap& bitmap);
	void TileBorderIntoMemDC(CDC& dcMem, CDC& dcBitmap, CRect rcArea, CBitmap& bmBorder,
									 BorderPts pts);
};

#endif // __BMPTILER_H__
