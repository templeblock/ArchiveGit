/*
 * Name:
 *	ADMDrawer.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	ADM Drawer Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 3/6/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __ADMDrawer__
#define __ADMDrawer__

/*
 * Includes
 */
 
#ifndef __ADMTypes__
#include "ADMTypes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma PRAGMA_ALIGN_BEGIN
#pragma PRAGMA_IMPORT_BEGIN

/*
 * Constants
 */

#define kADMDrawerSuite				"ADM Drawer Suite"

#define kADMDrawerSuiteVersion3		3
#define kADMDrawerSuiteVersion2		2

#define kADMDrawerSuiteVersion		kADMDrawerSuiteVersion3  // The current version


/*
 * Types
 */

typedef enum 
{
	kADMBlackColor = 0,
	kADMWhiteColor,
	kADMHiliteColor,
	kADMHiliteTextColor,
	kADMLightColor,
	kADMBackgroundColor,
	kADMShadowColor,
	kADMDisabledColor,
	kADMButtonUpColor,
	kADMButtonDownColor,
	kADMButtonDownShadowColor,
	kADMToolTipBackgroundColor,
	kADMToolTipForegroundColor,
	kADMWindowColor,
	kADMForegroundColor,
	kADMTextColor,
	kADMRedColor,
	kADMDummyColor = 0xFFFFFFFF
} ADMColor;

typedef enum
{
	kADMNormalMode = 0,
	kADMXORMode,
	kADMDummyMode = 0xFFFFFFFF
} ADMDrawMode;

typedef struct
{
	int height;
	int ascent;
	int descent;
	int leading;
	int maxWidth;
} ADMFontInfo;


typedef enum
{
	kADMNoRecolor,
	kADMActiveRecolor,
	kADMInactiveRecolor,
	kADMDisabledRecolor,
	kADMDummyRecolor = 0xFFFFFFFF
} ADMRecolorStyle;

/*
 * Drawer Suite
 */
typedef struct ADMDrawerSuite2
{
	ASPortRef ASAPI (*GetPortRef)(ADMDrawerRef drawer);
	
	void ASAPI (*Clear)(ADMDrawerRef drawer);
	
	void ASAPI (*GetBoundsRect)(ADMDrawerRef drawer, ASRect *boundsRect);
	
	
	void ASAPI (*GetClipRect)(ADMDrawerRef drawer, ASRect *clipRect);
	void ASAPI (*SetClipRect)(ADMDrawerRef drawer, ASRect *clipRect);
	void ASAPI (*IntersectClipRect)(ADMDrawerRef drawer, ASRect *clipRect);
	void ASAPI (*UnionClipRect)(ADMDrawerRef drawer, ASRect *clipRect);
	void ASAPI (*SubtractClipRect)(ADMDrawerRef drawer, ASRect *clipRect);

	void ASAPI (*SetClipPolygon)(ADMDrawerRef drawer, ASPoint *points, int numPoints);
	void ASAPI (*IntersectClipPolygon)(ADMDrawerRef drawer, ASPoint *points, int numPoints);
	void ASAPI (*UnionClipPolygon)(ADMDrawerRef drawer, ASPoint *points, int numPoints);
	void ASAPI (*SubtractClipPolygon)(ADMDrawerRef drawer, ASPoint *points, int numPoints);
	
	void ASAPI (*GetOrigin)(ADMDrawerRef drawer, ASPoint *origin);
	void ASAPI (*SetOrigin)(ADMDrawerRef drawer, ASPoint *origin);
	
	void ASAPI (*GetRGBColor)(ADMDrawerRef drawer, ASRGBColor *color);
	void ASAPI (*SetRGBColor)(ADMDrawerRef drawer, ASRGBColor *color);
	
	ADMColor ASAPI (*GetADMColor)(ADMDrawerRef drawer);
	void ASAPI (*SetADMColor)(ADMDrawerRef drawer, ADMColor color);
	
	ADMDrawMode ASAPI (*GetDrawMode)(ADMDrawerRef drawer);
	void ASAPI (*SetDrawMode)(ADMDrawerRef drawer, ADMDrawMode drawMode);
	
	ADMFont ASAPI (*GetFont)(ADMDrawerRef drawer);
	void ASAPI (*SetFont)(ADMDrawerRef drawer, ADMFont font);
	
	
	void ASAPI (*DrawLine)(ADMDrawerRef drawer, ASPoint *startPoint, ASPoint *endPoint);
	
	void ASAPI (*DrawPolygon)(ADMDrawerRef drawer, ASPoint *points, int numPoints);
	void ASAPI (*FillPolygon)(ADMDrawerRef drawer, ASPoint *points, int numPoints);
	
	void ASAPI (*DrawRect)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*FillRect)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*ClearRect)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*DrawSunkenRect)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*DrawRaisedRect)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*InvertRect)(ADMDrawerRef drawer, ASRect *rect);
	
	void ASAPI (*DrawOval)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*FillOval)(ADMDrawerRef drawer, ASRect *rect);
	
	_t_AGMPortPtr ASAPI (*GetAGMPort)(ADMDrawerRef drawer);
	void ASAPI (*DrawAGMImage)(ADMDrawerRef drawer, struct _t_AGMImageRecord *image, struct _t_AGMFixedMatrix *matrix, long flags);
	
	void ASAPI (*DrawResPicture)(ADMDrawerRef drawer, struct SPPlugin *pluginRef, int resID, ASPoint *topLeftPoint);
	void ASAPI (*DrawResPictureCentered)(ADMDrawerRef drawer, struct SPPlugin *pluginRef, int resID, ASRect *rect);
	
	void ASAPI (*DrawRecoloredResPicture)(ADMDrawerRef drawer, struct SPPlugin *pluginRef, int resID, ASPoint *topLeftPoint, ADMRecolorStyle style);
	void ASAPI (*DrawRecoloredResPictureCentered)(ADMDrawerRef drawer, struct SPPlugin *pluginRef, int resID, ASRect *rect, ADMRecolorStyle style);

	void ASAPI (*DrawIcon)(ADMDrawerRef drawer, ADMIconRef icon, ASPoint *topLeftPoint);
	void ASAPI (*DrawIconCentered)(ADMDrawerRef drawer, ADMIconRef icon, ASRect *rect);

	void ASAPI (*DrawRecoloredIcon)(ADMDrawerRef drawer, ADMIconRef icon, ASPoint *topLeftPoint, ADMRecolorStyle style);
	void ASAPI (*DrawRecoloredIconCentered)(ADMDrawerRef drawer, ADMIconRef icon, ASRect *rect, ADMRecolorStyle style);

	void ASAPI (*GetResPictureBounds)(ADMDrawerRef drawer, struct SPPlugin *pluginRef, int resID, ASRect *boundsRect);

	int ASAPI (*GetTextWidth)(ADMDrawerRef drawer, char *text);
	void ASAPI (*GetFontInfo)(ADMDrawerRef drawer, ADMFontInfo *fontInfo);
	
	void ASAPI (*DrawText)(ADMDrawerRef drawer, char *text, ASPoint *point);
	
	void ASAPI (*DrawTextLeft)(ADMDrawerRef drawer, char *text, ASRect *rect);
	void ASAPI (*DrawTextCentered)(ADMDrawerRef drawer, char *text, ASRect *rect);
	void ASAPI (*DrawTextRight)(ADMDrawerRef drawer, char *text, ASRect *rect);
	
	void ASAPI (*DrawUpArrow)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*DrawDownArrow)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*DrawLeftArrow)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*DrawRightArrow)(ADMDrawerRef drawer, ASRect *rect);

	ADMDrawerRef ASAPI (*Create)(ASPortRef portRef, ASRect *boundsRect, ADMFont font);	
	void ASAPI (*Destroy)(ADMDrawerRef drawer);

	void ASAPI (*DrawADMImage)(ADMDrawerRef drawer, ADMImageRef image, ASPoint *topLeftPoint);
	void ASAPI (*DrawADMImageCentered)(ADMDrawerRef drawer, ADMImageRef image, ASRect *rect);	
	
} ADMDrawerSuite2;


typedef struct ADMDrawerSuite3
{
	ASPortRef ASAPI (*GetPortRef)(ADMDrawerRef drawer);
	
	void ASAPI (*Clear)(ADMDrawerRef drawer);
	
	void ASAPI (*GetBoundsRect)(ADMDrawerRef drawer, ASRect *boundsRect);
	
	
	void ASAPI (*GetClipRect)(ADMDrawerRef drawer, ASRect *clipRect);
	void ASAPI (*SetClipRect)(ADMDrawerRef drawer, ASRect *clipRect);
	void ASAPI (*IntersectClipRect)(ADMDrawerRef drawer, ASRect *clipRect);
	void ASAPI (*UnionClipRect)(ADMDrawerRef drawer, ASRect *clipRect);
	void ASAPI (*SubtractClipRect)(ADMDrawerRef drawer, ASRect *clipRect);

	void ASAPI (*SetClipPolygon)(ADMDrawerRef drawer, ASPoint *points, int numPoints);
	void ASAPI (*IntersectClipPolygon)(ADMDrawerRef drawer, ASPoint *points, int numPoints);
	void ASAPI (*UnionClipPolygon)(ADMDrawerRef drawer, ASPoint *points, int numPoints);
	void ASAPI (*SubtractClipPolygon)(ADMDrawerRef drawer, ASPoint *points, int numPoints);
	
	void ASAPI (*GetOrigin)(ADMDrawerRef drawer, ASPoint *origin);
	void ASAPI (*SetOrigin)(ADMDrawerRef drawer, ASPoint *origin);
	
	void ASAPI (*GetRGBColor)(ADMDrawerRef drawer, ASRGBColor *color);
	void ASAPI (*SetRGBColor)(ADMDrawerRef drawer, ASRGBColor *color);
	
	ADMColor ASAPI (*GetADMColor)(ADMDrawerRef drawer);
	void ASAPI (*SetADMColor)(ADMDrawerRef drawer, ADMColor color);
	
	ADMDrawMode ASAPI (*GetDrawMode)(ADMDrawerRef drawer);
	void ASAPI (*SetDrawMode)(ADMDrawerRef drawer, ADMDrawMode drawMode);
	
	ADMFont ASAPI (*GetFont)(ADMDrawerRef drawer);
	void ASAPI (*SetFont)(ADMDrawerRef drawer, ADMFont font);
	
	
	void ASAPI (*DrawLine)(ADMDrawerRef drawer, ASPoint *startPoint, ASPoint *endPoint);
	
	void ASAPI (*DrawPolygon)(ADMDrawerRef drawer, ASPoint *points, int numPoints);
	void ASAPI (*FillPolygon)(ADMDrawerRef drawer, ASPoint *points, int numPoints);
	
	void ASAPI (*DrawRect)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*FillRect)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*ClearRect)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*DrawSunkenRect)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*DrawRaisedRect)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*InvertRect)(ADMDrawerRef drawer, ASRect *rect);
	
	void ASAPI (*DrawOval)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*FillOval)(ADMDrawerRef drawer, ASRect *rect);
	
	ASAPI struct _t_AGMPort* (*GetAGMPort)(ADMDrawerRef drawer);
	void ASAPI (*DrawAGMImage)(ADMDrawerRef drawer, struct _t_AGMImageRecord *image, struct _t_AGMFixedMatrix *matrix, long flags);
	
	void ASAPI (*DrawResPicture)(ADMDrawerRef drawer, struct SPPlugin *pluginRef, int resID, ASPoint *topLeftPoint);
	void ASAPI (*DrawResPictureCentered)(ADMDrawerRef drawer, struct SPPlugin *pluginRef, int resID, ASRect *rect);
	
	void ASAPI (*DrawRecoloredResPicture)(ADMDrawerRef drawer, struct SPPlugin *pluginRef, int resID, ASPoint *topLeftPoint, ADMRecolorStyle style);
	void ASAPI (*DrawRecoloredResPictureCentered)(ADMDrawerRef drawer, struct SPPlugin *pluginRef, int resID, ASRect *rect, ADMRecolorStyle style);

	void ASAPI (*DrawIcon)(ADMDrawerRef drawer, ADMIconRef icon, ASPoint *topLeftPoint);
	void ASAPI (*DrawIconCentered)(ADMDrawerRef drawer, ADMIconRef icon, ASRect *rect);

	void ASAPI (*DrawRecoloredIcon)(ADMDrawerRef drawer, ADMIconRef icon, ASPoint *topLeftPoint, ADMRecolorStyle style);
	void ASAPI (*DrawRecoloredIconCentered)(ADMDrawerRef drawer, ADMIconRef icon, ASRect *rect, ADMRecolorStyle style);

	void ASAPI (*GetResPictureBounds)(ADMDrawerRef drawer, struct SPPlugin *pluginRef, int resID, ASRect *boundsRect);

	int ASAPI (*GetTextWidth)(ADMDrawerRef drawer, char *text);
	void ASAPI (*GetFontInfo)(ADMDrawerRef drawer, ADMFontInfo *fontInfo);
	
	void ASAPI (*DrawText)(ADMDrawerRef drawer, char *text, ASPoint *point);
	
	void ASAPI (*DrawTextLeft)(ADMDrawerRef drawer, char *text, ASRect *rect);
	void ASAPI (*DrawTextCentered)(ADMDrawerRef drawer, char *text, ASRect *rect);
	void ASAPI (*DrawTextRight)(ADMDrawerRef drawer, char *text, ASRect *rect);
	
	void ASAPI (*DrawUpArrow)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*DrawDownArrow)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*DrawLeftArrow)(ADMDrawerRef drawer, ASRect *rect);
	void ASAPI (*DrawRightArrow)(ADMDrawerRef drawer, ASRect *rect);

	ADMDrawerRef ASAPI (*Create)(ASPortRef portRef, ASRect *boundsRect, ADMFont font);	
	void ASAPI (*Destroy)(ADMDrawerRef drawer);

	void ASAPI (*DrawADMImage)(ADMDrawerRef drawer, ADMImageRef image, ASPoint *topLeftPoint);
	void ASAPI (*DrawADMImageCentered)(ADMDrawerRef drawer, ADMImageRef image, ASRect *rect);	

	ASPortRef ASAPI (*GetADMWindowPort)(ASWindowRef window);
	void ASAPI (*ReleaseADMWindowPort)(ASWindowRef window, ASPortRef port);

	void ASAPI (*GetThisFontInfo)(ADMFont font, ADMFontInfo *fontInfo);
	void ASAPI (*DrawTextInaBox)(ADMDrawerRef drawer, ASRect *rect, char *text);

} ADMDrawerSuite3;

typedef ADMDrawerSuite3 ADMDrawerSuite;

#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif








