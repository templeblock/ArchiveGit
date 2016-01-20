//////////////////////////////////////////////////////////////////////////////
//
// $Header: /PM8/App/BmpTiler.cpp 1     3/03/99 6:03p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/BmpTiler.cpp $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 5     2/17/99 10:12a Mwilson
// modified tiling
// 
// 4     2/16/99 5:29p Mwilson
// added tiler that handles RDrawingSurface and RBitmapImage
// 
// 3     3/11/98 1:57p Hforman
// fixed potential crash bug if GetBitmap() fails
// 
// 2     3/06/98 1:07p Hforman
// 
// 1     3/06/98 11:03a Hforman
//
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BmpTiler.h"
#include "bitmapimage.h"

////////////////////////////////////////////////////////////////////////////
// CBmpTiler -- helper class that tiles a bitmap onto a larger area. Also
// tiles borders around the edges
//
////////////////////////////////////////////////////////////////////////////

CBmpTiler::CBmpTiler()
{
}

CBmpTiler::~CBmpTiler()
{
}

void CBmpTiler::TileBmpInRect(CDC& dc, const CRect& rcArea, CBitmap& bmTile)
{
	DoTile(dc, rcArea, bmTile);
}

void CBmpTiler::TileBmpInRect(RDrawingSurface& drawingSurface, const CRect& rcBkgndPortion, RBitmapImage* pImage)
{
	DoTile(drawingSurface, rcBkgndPortion, pImage);
}

void CBmpTiler::TileBmpAndBorderInRect(CDC& dc, const CRect& rcArea, CBitmap& bmTile,
													CBitmap& bmBorder, BorderPts pts)
{
	DoTile(dc, rcArea, bmTile, TRUE, &bmBorder, &pts);
}

void CBmpTiler::DoTile(RDrawingSurface& drawingSurface, const CRect& rcBkgndPortion, RBitmapImage* pImage,
					BOOL fHasBorder/*=FALSE*/, RBitmapImage* pBorderImage/*=NULL*/, BorderPts* pts/*=NULL*/)
{
	//this function assumes that the portion of the drawing surface that you want
	//rendered into starts at 0,0 and is the same size as rcBkgndPortion 
	//(the destination rect is (0,0, rcBkgndPortion.Width, rcBkgndPortion.Height)

	// get bitmap size
	int bmWidth = pImage->GetWidthInPixels();
	int bmHeight = pImage->GetHeightInPixels();

	//determine starting and ending columns and rows
	int nStartColumn = rcBkgndPortion.left / bmWidth;
	int nEndColumn = rcBkgndPortion.right / bmWidth + 1;
	int nStartRow = rcBkgndPortion.top / bmHeight;
	int nEndRow = rcBkgndPortion.bottom / bmHeight + 1;

	//src rect
	CRect rcImage(0,0,bmWidth, bmHeight);
	//calculate starting tile rect
	CRect rcStartTile(rcImage);
	rcStartTile.OffsetRect(CPoint(nStartColumn * bmWidth, nStartRow * bmHeight) - rcBkgndPortion.TopLeft());

	//we may need to set a clipping rect the size of rcBkgndProtion
	CRect rcTile;
	for (int nRow = nStartRow; nRow < nEndRow; nRow++)
	{
		//For each new row go back to the start tile so the x coords are correct
		rcTile = rcStartTile;
		//set the tile rect to the beginning of the new row.  This sets the y coords.
		rcTile.OffsetRect(0, (nRow - nStartRow) * bmHeight);		
		for (int nColumn = nStartColumn; nColumn < nEndColumn; nColumn++)
		{
			pImage->Render(drawingSurface, rcImage, rcTile);
			//offsetting by image width gives us the next tile rect
			rcTile.OffsetRect(bmWidth, 0);
		}
	}
}

void CBmpTiler::DoTile(CDC& dc, const CRect& rcArea, CBitmap& bmTile,
							  BOOL fHasBorder, CBitmap* pbmBorder, BorderPts* pts)
{
	CDC dcBitmap, dcMem;
	if (dcBitmap.CreateCompatibleDC(&dc) && dcMem.CreateCompatibleDC(&dc))
	{
		CBitmap bmArea;
		bmArea.CreateCompatibleBitmap(&dc, rcArea.Width(), rcArea.Height());
		CBitmap* pOldMemBmp = dcMem.SelectObject(&bmArea);

		TileBmpIntoMemDC(dcMem, dcBitmap, rcArea, bmTile);

		if (fHasBorder && pbmBorder != NULL && pts != NULL)
			TileBorderIntoMemDC(dcMem, dcBitmap, rcArea, *pbmBorder, *pts);

		dc.BitBlt(rcArea.left, rcArea.top, rcArea.Width(), rcArea.Height(),
					 &dcMem, 0, 0, SRCCOPY);

		dcMem.SelectObject(pOldMemBmp);
	}
}

void CBmpTiler::TileBmpIntoMemDC(CDC& dcMem, CDC& dcBitmap, CRect rcArea, CBitmap& bmTile)
{
	// get bitmap size
	int bmWidth, bmHeight;
	BITMAP bmInfo;
	if (bmTile.GetBitmap(&bmInfo))
	{
		bmWidth = bmInfo.bmWidth;
		bmHeight = bmInfo.bmHeight;
	}
	else
	{
		ASSERT(0);
		return;
	}
	
	CBitmap* pOldBmp = dcBitmap.SelectObject(&bmTile);

	// tile background bitmap
	int rows = rcArea.Height() / bmHeight + 1;
	int tiles_per_row = rcArea.Width() / bmWidth + 1;

	for (int row = 0; row < rows; row++)
	{
		for (int tile = 0; tile < tiles_per_row; tile++)
			dcMem.BitBlt(tile * bmWidth, row * bmHeight, bmWidth, bmHeight, &dcBitmap,
							 0, 0, SRCCOPY);
	}

	// deselect the bitmap
	if (pOldBmp)
		dcBitmap.SelectObject(pOldBmp);
}

void CBmpTiler::TileBorderIntoMemDC(CDC& dcMem, CDC& dcBitmap, CRect rcArea,
												CBitmap& bmBorder, BorderPts pts)
{
	CBitmap* pOldBmp = dcBitmap.SelectObject(&bmBorder);

	// get bitmap size
	int bmWidth = 0;
	int bmHeight = 0;
	BITMAP bmInfo;
	if (bmBorder.GetBitmap(&bmInfo))
	{
		bmWidth = bmInfo.bmWidth;
		bmHeight = bmInfo.bmHeight;
	}

	int topHeight = pts.nHorzUpper;
	int btmHeight = bmHeight - pts.nHorzLower;
	int leftWidth = pts.nVertLeft;
	int rghtWidth = bmWidth - pts.nVertRight;

	// blt the 4 corners...
	// top left
	dcMem.BitBlt(0, 0,						// dst x, y
					 leftWidth, topHeight,	// src/dst width, height
					 &dcBitmap,
					 0, 0,						// src x, y
					 SRCCOPY);
	// top right
	dcMem.BitBlt(rcArea.Width() - rghtWidth, 0,
					 rghtWidth, topHeight,
					 &dcBitmap,
					 pts.nVertRight, 0,
					 SRCCOPY);
	// bottom left
	dcMem.BitBlt(0, rcArea.Height() - btmHeight,
					 leftWidth, btmHeight,
					 &dcBitmap,
					 0, pts.nHorzLower,
					 SRCCOPY);
	// bottom right
	dcMem.BitBlt(rcArea.Width() - rghtWidth, rcArea.Height() - btmHeight,
					 rghtWidth, btmHeight,
					 &dcBitmap,
					 bmWidth - rghtWidth, bmHeight - btmHeight,
					 SRCCOPY);

	// tile edges...
	int widthOfCorners = leftWidth + rghtWidth;
	int tileWidth = bmWidth - widthOfCorners;
	int nTiles = ((rcArea.Width() - widthOfCorners) / tileWidth) + 1;
	int lastWidth = rcArea.Width() - ((nTiles - 1) * tileWidth) - widthOfCorners;

	// top edge
	for (int tile = 0; tile < nTiles; tile++)
	{
		int width = (tile == nTiles - 1) ? lastWidth : tileWidth;
		dcMem.BitBlt(tile * tileWidth + pts.nVertLeft, 0,
						 width, topHeight,
						 &dcBitmap,
						 pts.nVertLeft, 0,
						 SRCCOPY);
	}

	// bottom edge
	for (tile = 0; tile < nTiles; tile++)
	{
		int width = (tile == nTiles - 1) ? lastWidth : tileWidth;
		dcMem.BitBlt(tile * tileWidth + pts.nVertLeft, rcArea.Height() - btmHeight,
						 width, btmHeight,
						 &dcBitmap,
						 pts.nVertLeft, pts.nHorzLower,
						 SRCCOPY);
	}

	// left edge
	int heightOfCorners = topHeight + btmHeight;
	int tileHeight = bmHeight - heightOfCorners;
	nTiles = ((rcArea.Height() - heightOfCorners) / tileHeight) + 1;
	int lastHeight = rcArea.Height() - ((nTiles - 1) * tileHeight) - heightOfCorners;

	for (tile = 0; tile < nTiles; tile++)
	{
		int height = (tile == nTiles - 1) ? lastHeight : tileHeight;
		dcMem.BitBlt(0, tile * tileHeight + topHeight,
						 leftWidth, height,
						 &dcBitmap,
						 0, pts.nHorzUpper,
						 SRCCOPY);
	}

	// right edge
	for (tile = 0; tile < nTiles; tile++)
	{
		int height = (tile == nTiles - 1) ? lastHeight : tileHeight;
		dcMem.BitBlt(rcArea.Width() - rghtWidth, tile * tileHeight + topHeight,
						 rghtWidth, height,
						 &dcBitmap,
						 bmWidth - rghtWidth, pts.nHorzUpper,
						 SRCCOPY);
	}

	// deselect the bitmap
	if (pOldBmp)
		dcBitmap.SelectObject(pOldBmp);
}

