/*
 * Name:
 *	BaseADMImageItem.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Dialog Manager Image Item Base Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 3/6/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __BaseADMImageItem_hpp__
#define __BaseADMImageItem_hpp__

/*
 * Includes
 */
 
#ifndef __BaseADMItem_hpp__
#include "BaseADMItem.hpp"
#endif

#ifndef __IADMImage_hpp__
#include "IADMImage.hpp"
#endif

class IADMDrawer;
class IADMNotifier;
class IADMTracker;


/*
 * Base Class
 */

class BaseADMImageItem : public BaseADMItem
{

protected:
	IADMImage fImage;

	virtual ASBoolean UseImage();
	virtual void BaseDraw(IADMDrawer drawer);

public:	
	BaseADMImageItem(IADMItem item, ADMUserData data = nil);
	
	virtual ~BaseADMImageItem();
};

#endif
