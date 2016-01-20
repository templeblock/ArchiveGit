/*
 * Name:
 *	BaseADMImageItem.cpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Item Manager Image Item Interface Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.1 3/11/1996	DL	Second version.
 *		Created by Dave Lazarony.
 */

/*
 * Includes
 */

#include "BaseADMImageItem.hpp"
#include "IADMDrawer.hpp"
#include "IASGlobalContext.hpp"

#include "AGMImage.h"

/*
 * Constructor
 */

BaseADMImageItem::BaseADMImageItem(IADMItem item, ADMUserData data) : BaseADMItem(item, data)								
{
	fImage = nil;
}


/*
 * Destructor
 */

BaseADMImageItem::~BaseADMImageItem()
{
	if (fImage && sADMImage)
	{
		sADMImage->Destroy(fImage);
		fImage = nil;
	}
}


/*
 * Virtual Functions that call Defaults.
 */
	
/*
 * Static Procs that call Virtual Functions.
 */
	
ASBoolean BaseADMImageItem::UseImage()
{
	return true;
}

void BaseADMImageItem::BaseDraw(IADMDrawer drawer)
{
	ASBoolean needNormalDraw = true;

	if (UseImage())
	{
		IASRect boundsRect;

		drawer.GetBoundsRect(boundsRect);
	
		if (fImage && (fImage.GetWidth() != boundsRect.Width() || fImage.GetHeight() != boundsRect.Height()))
		{
			sADMImage->Destroy(fImage);
			fImage = nil;
		}

		if (!fImage)
			fImage = sADMImage->CreateOffscreen(boundsRect.Width(), boundsRect.Height());
		
		if (fImage)	
		{
			IADMDrawer imageDrawer = fImage.BeginADMDrawer();
			
			if (imageDrawer)
			{
				imageDrawer.ClearRect(boundsRect);
				
				Draw(imageDrawer);

				fImage.EndADMDrawer();

				needNormalDraw = false;
			}
			
			IASPoint point(0, 0);

			drawer.DrawADMImage(fImage, point);
		}
	}
	
	if (needNormalDraw)
		Draw(drawer);
}
