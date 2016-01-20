/*
 * Name:
 *	IADMDrawer.hpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	IADMDrawer Wrapper Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 3/6/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __IADMDrawer_hpp__
#define __IADMDrawer_hpp__

/*
 * Includes
 */
 
#ifndef __ADMDrawer__
#include "ADMDrawer.h"
#endif

#ifndef __IASTypes_hpp__
#include "IASTypes.hpp"
#endif


/*
 * Forward Declarations
 */
 
extern "C" typedef struct SPPlugin SPPlugin;
extern "C" typedef struct _t_AGMImageRecord AGMImageRecord;
extern "C" typedef struct _t_AGMFixedMatrix AGMFixedMatrix;
class AGMPort;


/*
 * Global Suite Pointer
 */

extern "C" ADMDrawerSuite *sADMDrawer;


/*
 * Wrapper Class
 */

class IADMDrawer
{
private:
	ADMDrawer *fDrawer;

public:	
	IADMDrawer();
	IADMDrawer(ADMDrawer *c);
	
	operator ADMDrawer *(void);
	
	
	ASPortRef GetPortRef();
	
	void Clear();
	
	void GetBoundsRect(IASRect &boundsRect);
	
	
	void GetClipRect(IASRect &clipRect);
	void SetClipRect(const IASRect &clipRect);
	void IntersectClipRect(const IASRect &clipRect);
	void UnionClipRect(const IASRect &clipRect);
	void SubtractClipRect(const IASRect &clipRect);

	void SetClipPolygon(const IASPoint *points, int numPoints);
	void IntersectClipPolygon(const IASPoint *points, int numPoints);
	void UnionClipPolygon(const IASPoint *points, int numPoints);
	void SubtractClipPolygon(const IASPoint *points, int numPoints);

	void GetOrigin(IASPoint &origin);
	void SetOrigin(const IASPoint &origin);
	
	void GetRGBColor(ASRGBColor &color);
	void SetRGBColor(const ASRGBColor &color);
	
	ADMColor GetADMColor();
	void SetADMColor(ADMColor color);
	
	ADMDrawMode GetDrawMode();
	void SetDrawMode(ADMDrawMode drawMode);
	
	ADMFont GetFont();
	void SetFont(ADMFont font);
	
	
	void DrawLine(const IASPoint &startPoint, const IASPoint &endPoint);
	
	void DrawPolygon(const IASPoint *points, int numPoints);
	void FillPolygon(const IASPoint *points, int numPoints);
	
	void DrawRect(const IASRect &rect);
	void FillRect(const IASRect &rect);
	void ClearRect(const IASRect &rect);
	void DrawSunkenRect(const IASRect &rect);
	void DrawRaisedRect(const IASRect &rect);
	void InvertRect(const IASRect &rect);
	
	void DrawOval(const IASRect &rect);
	void FillOval(const IASRect &rect);
	
	AGMPort* GetAGMPort();
	void DrawAGMImage(AGMImageRecord *image, AGMFixedMatrix *matrix, long flags);

	void DrawADMImage(ADMImageRef image, const IASPoint &topLeftPoint);
	void DrawADMImageCentered(ADMImageRef image, const IASRect &rect);

	void DrawResPicture(SPPlugin *pluginRef, int resID, const IASPoint &topLeftPoint, ADMRecolorStyle style = kADMNoRecolor);
	void DrawResPictureCentered(SPPlugin *pluginRef, int resID, const IASRect &rect, ADMRecolorStyle style = kADMNoRecolor);

	void DrawIcon(ADMIconRef icon, const IASPoint &topLeftPoint, ADMRecolorStyle style = kADMNoRecolor);
	void DrawIconCentered(ADMIconRef icon, const IASRect &rect, ADMRecolorStyle style = kADMNoRecolor);

	void GetResPictureBounds(struct SPPlugin *pluginRef, int resID, IASRect &boundsRect);

	int GetTextWidth(const char *text);
	void GetFontInfo(const ADMFontInfo &fontInfo);
	
	void DrawText(const char *text, const IASPoint &point);
	
	void DrawTextLeft(const char *text, const IASRect &rect);
	void DrawTextCentered(const char *text, const IASRect &rect);
	void DrawTextRight(const char *text, const IASRect &rect);
	
	void DrawUpArrow(const IASRect &rect);
	void DrawDownArrow(const IASRect &rect);
	void DrawLeftArrow(const IASRect &rect);
	void DrawRightArrow(const IASRect &rect);	
};

inline IADMDrawer::IADMDrawer()								
{
}

inline IADMDrawer::IADMDrawer(ADMDrawer *drawer)	
{
	fDrawer = drawer;
}

inline IADMDrawer::operator ADMDrawer *(void)
{
	return fDrawer;
}

inline ASPortRef IADMDrawer::GetPortRef()
{
	return sADMDrawer->GetPortRef(fDrawer);
}

inline void IADMDrawer::Clear()
{
	sADMDrawer->Clear(fDrawer);
}
	
inline void IADMDrawer::GetBoundsRect(IASRect &boundsRect)
{
	sADMDrawer->GetBoundsRect(fDrawer, &boundsRect);
}

inline void IADMDrawer::GetClipRect(IASRect &clipRect)
{
	sADMDrawer->GetClipRect(fDrawer, &clipRect);
}

inline void IADMDrawer::SetClipRect(const IASRect &clipRect)
{
	sADMDrawer->SetClipRect(fDrawer, (IASRect *)&clipRect);
}

inline void IADMDrawer::IntersectClipRect(const IASRect &clipRect)
{
	sADMDrawer->IntersectClipRect(fDrawer, (IASRect *)&clipRect);
}

inline void IADMDrawer::UnionClipRect(const IASRect &clipRect)
{
	sADMDrawer->UnionClipRect(fDrawer, (IASRect *)&clipRect);
}

inline void IADMDrawer::SubtractClipRect(const IASRect &clipRect)
{
	sADMDrawer->SubtractClipRect(fDrawer, (IASRect *)&clipRect);
}

inline void IADMDrawer::SetClipPolygon(const IASPoint *points, int numPoints)
{
	sADMDrawer->SetClipPolygon(fDrawer, (IASPoint *)points, numPoints);
}

inline void IADMDrawer::IntersectClipPolygon(const IASPoint *points, int numPoints)
{
	sADMDrawer->IntersectClipPolygon(fDrawer, (IASPoint *)points, numPoints);
}

inline void IADMDrawer::UnionClipPolygon(const IASPoint *points, int numPoints)
{
	sADMDrawer->UnionClipPolygon(fDrawer, (IASPoint *)points, numPoints);
}

inline void IADMDrawer::SubtractClipPolygon(const IASPoint *points, int numPoints)
{
	sADMDrawer->SubtractClipPolygon(fDrawer, (IASPoint *)points, numPoints);
}

inline void IADMDrawer::GetOrigin(IASPoint &origin)
{
	sADMDrawer->GetOrigin(fDrawer, &origin);
}

inline void IADMDrawer::SetOrigin(const IASPoint &origin)
{
	sADMDrawer->SetOrigin(fDrawer, (IASPoint *)&origin);
}

inline void IADMDrawer::GetRGBColor(ASRGBColor &color)
{
	sADMDrawer->GetRGBColor(fDrawer, &color);
}

inline void IADMDrawer::SetRGBColor(const ASRGBColor &color)
{
	sADMDrawer->SetRGBColor(fDrawer, (ASRGBColor *)&color);
}

inline ADMColor IADMDrawer::GetADMColor()
{
	return sADMDrawer->GetADMColor(fDrawer);
}

inline void IADMDrawer::SetADMColor(ADMColor color)
{
	sADMDrawer->SetADMColor(fDrawer, color);
}

inline ADMDrawMode IADMDrawer::GetDrawMode()
{
	return sADMDrawer->GetDrawMode(fDrawer);
}

inline void IADMDrawer::SetDrawMode(ADMDrawMode drawMode)
{
	sADMDrawer->SetDrawMode(fDrawer, drawMode);
}

inline ADMFont IADMDrawer::GetFont()
{
	return sADMDrawer->GetFont(fDrawer);
}

inline void IADMDrawer::SetFont(ADMFont font)
{
	sADMDrawer->SetFont(fDrawer, font);
}

inline void IADMDrawer::DrawLine(const IASPoint &startPoint, const IASPoint &endPoint)
{
	sADMDrawer->DrawLine(fDrawer, (IASPoint *)&startPoint, (IASPoint *)&endPoint);
}

inline void IADMDrawer::DrawPolygon(const IASPoint *points, int numPoints)
{
	sADMDrawer->DrawPolygon(fDrawer, (IASPoint *)points, numPoints);
}

inline void IADMDrawer::FillPolygon(const IASPoint *points, int numPoints)
{
	sADMDrawer->FillPolygon(fDrawer, (IASPoint *)points, numPoints);
}

inline void IADMDrawer::DrawRect(const IASRect &rect)
{
	sADMDrawer->DrawRect(fDrawer, (IASRect *)&rect);
}

inline void IADMDrawer::FillRect(const IASRect &rect)
{
	sADMDrawer->FillRect(fDrawer, (IASRect *)&rect);
}

inline void IADMDrawer::ClearRect(const IASRect &rect)
{
	sADMDrawer->ClearRect(fDrawer, (IASRect *)&rect);
}

inline void IADMDrawer::DrawSunkenRect(const IASRect &rect)
{
	sADMDrawer->DrawSunkenRect(fDrawer, (IASRect *)&rect);
}

inline void IADMDrawer::DrawRaisedRect(const IASRect &rect)
{
	sADMDrawer->DrawRaisedRect(fDrawer, (IASRect *)&rect);
}

inline void IADMDrawer::InvertRect(const IASRect &rect)
{
	sADMDrawer->InvertRect(fDrawer, (IASRect *)&rect);
}

inline void IADMDrawer::DrawOval(const IASRect &rect)
{
	sADMDrawer->DrawOval(fDrawer, (IASRect *)&rect);
}

inline void IADMDrawer::FillOval(const IASRect &rect)
{
	sADMDrawer->FillOval(fDrawer, (IASRect *)&rect);
}

inline AGMPort* IADMDrawer::GetAGMPort()
{
	return (AGMPort*) sADMDrawer->GetAGMPort(fDrawer);
}

inline void IADMDrawer::DrawAGMImage(AGMImageRecord *image, AGMFixedMatrix *matrix, long flags)
{
	sADMDrawer->DrawAGMImage(fDrawer, (struct _t_AGMImageRecord *)image, (struct _t_AGMFixedMatrix *)matrix, flags);
}

inline void IADMDrawer::DrawADMImage(ADMImageRef image, const IASPoint &topLeftPoint)
{
	sADMDrawer->DrawADMImage(fDrawer, image, (IASPoint *)&topLeftPoint);
}

inline void IADMDrawer::DrawADMImageCentered(ADMImageRef image, const IASRect &rect)
{
	sADMDrawer->DrawADMImageCentered(fDrawer, image, (IASRect *)&rect);
}

inline void IADMDrawer::DrawResPicture(SPPlugin *pluginRef, int resID, const IASPoint &topLeftPoint, ADMRecolorStyle style)
{
	sADMDrawer->DrawRecoloredResPicture(fDrawer, pluginRef, resID, (IASPoint *)&topLeftPoint, style);
}

inline void IADMDrawer::DrawResPictureCentered(SPPlugin *pluginRef, int resID, const IASRect &rect, ADMRecolorStyle style)
{
	sADMDrawer->DrawRecoloredResPictureCentered(fDrawer, pluginRef, resID, (IASRect *)&rect, style);
}

inline void IADMDrawer::DrawIcon(ADMIconRef icon, const IASPoint &topLeftPoint, ADMRecolorStyle style)
{
	sADMDrawer->DrawRecoloredIcon(fDrawer, icon, (IASPoint *)&topLeftPoint, style);
}

inline void IADMDrawer::DrawIconCentered(ADMIconRef icon, const IASRect &rect, ADMRecolorStyle style)
{
	sADMDrawer->DrawRecoloredIconCentered(fDrawer, icon, (IASRect *)&rect, style);
}

inline void IADMDrawer::GetResPictureBounds(struct SPPlugin *pluginRef, int resID, IASRect &boundsRect)
{
	sADMDrawer->GetResPictureBounds(fDrawer, pluginRef, resID, (IASRect *)&boundsRect);
}

inline int IADMDrawer::GetTextWidth(const char *text)
{
	return sADMDrawer->GetTextWidth(fDrawer, (char *)text);
}

inline void IADMDrawer::GetFontInfo(const ADMFontInfo &fontInfo)
{
	sADMDrawer->GetFontInfo(fDrawer, (ADMFontInfo*)&fontInfo);
}

inline void IADMDrawer::DrawText(const char *text, const IASPoint &point)
{
	sADMDrawer->DrawText(fDrawer, (char *)text, (IASPoint *)&point);
}

inline void IADMDrawer::DrawTextLeft(const char *text, const IASRect &rect)
{
	sADMDrawer->DrawTextLeft(fDrawer, (char *)text, (IASRect *)&rect);
}

inline void IADMDrawer::DrawTextCentered(const char *text, const IASRect &rect)
{
	sADMDrawer->DrawTextCentered(fDrawer, (char *)text, (IASRect *)&rect);
}

inline void IADMDrawer::DrawTextRight(const char *text, const IASRect &rect)
{
	sADMDrawer->DrawTextRight(fDrawer, (char *)text, (IASRect *)&rect);
}

inline void IADMDrawer::DrawUpArrow(const IASRect &rect)
{
	sADMDrawer->DrawUpArrow(fDrawer, (IASRect *)&rect);
}	

inline void IADMDrawer::DrawDownArrow(const IASRect &rect)
{
	sADMDrawer->DrawDownArrow(fDrawer, (IASRect *)&rect);
}	

inline void IADMDrawer::DrawLeftArrow(const IASRect &rect)
{
	sADMDrawer->DrawLeftArrow(fDrawer, (IASRect *)&rect);
}	

inline void IADMDrawer::DrawRightArrow(const IASRect &rect)
{
	sADMDrawer->DrawRightArrow(fDrawer, (IASRect *)&rect);
}	

#endif








