/*
// $Workfile: bkgobj.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/bkgobj.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 5     2/01/99 2:01p Johno
// Backgrounds now flip
// 
// 4     2/01/99 9:38a Johno
// Changes for paneled HTML
// 
// 3     1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// Preliminary separate panel backgrounds
// 
// 2     2/24/98 9:15a Dennis
// More complete background object
// 
// 1     2/17/98 9:09a Dennis
// First Revision
// 
*/

#ifndef __BKGOBJ_H__
#define __BKGOBJ_H__

class CDIBInfo;

class CBackgroundObject : public RectPageObject
{
	INHERIT(CBackgroundObject, RectPageObject)
public:

	CBackgroundObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);
	~CBackgroundObject();

   void              Empty();

   void SetTextureRecNum(DB_RECORD_NUMBER dbNewRecNum, BOOL Flipped = FALSE);
/*
// Update an object into a redisplay context.
*/
	UpdateStatePtr update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type);

	virtual BOOL is_opaque(RedisplayContextPtr rc)
   {
      return HasBackground();
   }

	DB_RECORD_NUMBER GetTextureRecNum()
	{ 
		return m_dbRecNumTexture; 
	}

	COLOR GetBackgroundColor()
	{ 
		return m_colorBackground; 
	}

	void SetBackgroundColor(COLOR newColor)
	{ 
		m_colorBackground = newColor; 
	}

	GraphicObjectPtr GetGraphicObject()
	{ 
		return m_pGraphicObject; 
	}

	BOOL HasTexture()
	{ 
		return m_dbRecNumTexture > 0; 
	}

	BOOL HasColor()
	{ 
		return (GetBackgroundColor() != UNDEFINED_COLOR && GetBackgroundColor() != COLOR_WHITE); 
	}

	BOOL HasBackground()
	{ 
		return (HasTexture() || HasColor()); 
	}

protected:
// Helpers
   void FreeDIB();
   void FreeGraphicObject();
   ERRORCODE CreateDIB(RedisplayContextPtr rc);

protected:
// Data Members
   DB_RECORD_NUMBER     m_dbRecNumTexture;
   DB_RECORD_NUMBER     m_dbRecNumDIB;
   COLOR                m_colorBackground;
   GraphicObjectPtr     m_pGraphicObject;
   CDIBInfo					m_cdibInfo;
};
#endif					// #ifndef __BKGOBJ_H__

