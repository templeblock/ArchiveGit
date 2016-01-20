// Create by Jim McCurdy
// Don't include this file directly; include Gdip.h
// This is a clone of <GdiPlusFlat.h> except:
// o functions are converted to typedefs
// o a big declaration list was added

/**************************************************************************\
*
* Copyright (c) 1998-2001, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   GdiplusFlat.h
*
* Abstract:
*
*   Private GDI+ header file.
*
\**************************************************************************/

#ifndef _FLATAPI_H
#define _FLATAPI_H

#define WINGDIPAPI __stdcall

#define GDIPCONST const

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// GraphicsPath APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreatePath)(GpFillMode brushMode, GpPath **path);

typedef GpStatus (WINGDIPAPI *PFNGdipCreatePath2)(GDIPCONST GpPointF*, GDIPCONST BYTE*, INT, GpFillMode,
                                    GpPath **path);

typedef GpStatus (WINGDIPAPI *PFNGdipCreatePath2I)(GDIPCONST GpPoint*, GDIPCONST BYTE*, INT, GpFillMode,
                                     GpPath **path);

typedef GpStatus (WINGDIPAPI *PFNGdipClonePath)(GpPath* path, GpPath **clonePath);

typedef GpStatus (WINGDIPAPI *PFNGdipDeletePath)(GpPath* path);

typedef GpStatus (WINGDIPAPI *PFNGdipResetPath)(GpPath* path);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPointCount)(GpPath* path, INT* count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathTypes)(GpPath* path, BYTE* types, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathPoints)(GpPath*, GpPointF* points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathPointsI)(GpPath*, GpPoint* points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathFillMode)(GpPath *path, GpFillMode *fillmode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathFillMode)(GpPath *path, GpFillMode fillmode);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathData)(GpPath *path, GpPathData* pathData);

typedef GpStatus (WINGDIPAPI *PFNGdipStartPathFigure)(GpPath *path);

typedef GpStatus (WINGDIPAPI *PFNGdipClosePathFigure)(GpPath *path);

typedef GpStatus (WINGDIPAPI *PFNGdipClosePathFigures)(GpPath *path);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathMarker)(GpPath* path);

typedef GpStatus (WINGDIPAPI *PFNGdipClearPathMarkers)(GpPath* path);

typedef GpStatus (WINGDIPAPI *PFNGdipReversePath)(GpPath* path);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathLastPoint)(GpPath* path, GpPointF* lastPoint);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathLine)(GpPath *path, REAL x1, REAL y1, REAL x2, REAL y2);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathLine2)(GpPath *path, GDIPCONST GpPointF *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathArc)(GpPath *path, REAL x, REAL y, REAL width, REAL height,
                        REAL startAngle, REAL sweepAngle);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathBezier)(GpPath *path, REAL x1, REAL y1, REAL x2, REAL y2,
                           REAL x3, REAL y3, REAL x4, REAL y4);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathBeziers)(GpPath *path, GDIPCONST GpPointF *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathCurve)(GpPath *path, GDIPCONST GpPointF *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathCurve2)(GpPath *path, GDIPCONST GpPointF *points, INT count,
                           REAL tension);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathCurve3)(GpPath *path, GDIPCONST GpPointF *points, INT count,
                           INT offset, INT numberOfSegments, REAL tension);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathClosedCurve)(GpPath *path, GDIPCONST GpPointF *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathClosedCurve2)(GpPath *path, GDIPCONST GpPointF *points, INT count,
                                 REAL tension);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathRectangle)(GpPath *path, REAL x, REAL y, REAL width, REAL height);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathRectangles)(GpPath *path, GDIPCONST GpRectF *rects, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathEllipse)(GpPath *path, REAL x, REAL y, REAL width,
                            REAL height);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathPie)(GpPath *path, REAL x, REAL y, REAL width, REAL height,
                        REAL startAngle, REAL sweepAngle);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathPolygon)(GpPath *path, GDIPCONST GpPointF *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathPath)(GpPath *path, GDIPCONST GpPath* addingPath, BOOL connect);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathString)(GpPath *path, GDIPCONST WCHAR *string,
                        INT length, GDIPCONST GpFontFamily *family, INT style,
                        REAL emSize, GDIPCONST RectF *layoutRect,
                        GDIPCONST GpStringFormat *format);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathStringI)(GpPath *path, GDIPCONST WCHAR *string,
                        INT length, GDIPCONST GpFontFamily *family, INT style,
                        REAL emSize, GDIPCONST Rect *layoutRect,
                        GDIPCONST GpStringFormat *format);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathLineI)(GpPath *path, INT x1, INT y1, INT x2, INT y2);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathLine2I)(GpPath *path, GDIPCONST GpPoint *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathArcI)(GpPath *path, INT x, INT y, INT width, INT height,
                        REAL startAngle, REAL sweepAngle);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathBezierI)(GpPath *path, INT x1, INT y1, INT x2, INT y2,
                           INT x3, INT y3, INT x4, INT y4);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathBeziersI)(GpPath *path, GDIPCONST GpPoint *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathCurveI)(GpPath *path, GDIPCONST GpPoint *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathCurve2I)(GpPath *path, GDIPCONST GpPoint *points, INT count,
                           REAL tension);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathCurve3I)(GpPath *path, GDIPCONST GpPoint *points, INT count,
                           INT offset, INT numberOfSegments, REAL tension);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathClosedCurveI)(GpPath *path, GDIPCONST GpPoint *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathClosedCurve2I)(GpPath *path, GDIPCONST GpPoint *points, INT count,
                                 REAL tension);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathRectangleI)(GpPath *path, INT x, INT y, INT width, INT height);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathRectanglesI)(GpPath *path, GDIPCONST GpRect *rects, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathEllipseI)(GpPath *path, INT x, INT y, INT width, INT height);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathPieI)(GpPath *path, INT x, INT y, INT width, INT height,
                        REAL startAngle, REAL sweepAngle);

typedef GpStatus (WINGDIPAPI *PFNGdipAddPathPolygonI)(GpPath *path, GDIPCONST GpPoint *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipFlattenPath)(GpPath *path, GpMatrix* matrix, REAL flatness);

typedef GpStatus (WINGDIPAPI *PFNGdipWindingModeOutline)(
    GpPath *path,
    GpMatrix *matrix,
    REAL flatness
);

typedef GpStatus (WINGDIPAPI *PFNGdipWidenPath)(
    GpPath *nativePath,
    GpPen *pen,
    GpMatrix *matrix,
    REAL flatness
);

typedef GpStatus (WINGDIPAPI *PFNGdipWarpPath)(GpPath *path, GpMatrix* matrix,
            GDIPCONST GpPointF *points, INT count,
            REAL srcx, REAL srcy, REAL srcwidth, REAL srcheight,
            WarpMode warpMode, REAL flatness);

typedef GpStatus (WINGDIPAPI *PFNGdipTransformPath)(GpPath* path, GpMatrix* matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathWorldBounds)(GpPath* path, GpRectF* bounds, 
                       GDIPCONST GpMatrix *matrix, GDIPCONST GpPen *pen);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathWorldBoundsI)(GpPath* path, GpRect* bounds, 
                        GDIPCONST GpMatrix *matrix, GDIPCONST GpPen *pen);

typedef GpStatus (WINGDIPAPI *PFNGdipIsVisiblePathPoint)(GpPath* path, REAL x, REAL y,
                       GpGraphics *graphics, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsVisiblePathPointI)(GpPath* path, INT x, INT y,
                        GpGraphics *graphics, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsOutlineVisiblePathPoint)(GpPath* path, REAL x, REAL y, GpPen *pen,
                              GpGraphics *graphics, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsOutlineVisiblePathPointI)(GpPath* path, INT x, INT y, GpPen *pen,
                               GpGraphics *graphics, BOOL *result);


//----------------------------------------------------------------------------
// PathIterator APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreatePathIter)(GpPathIterator **iterator, GpPath* path);

typedef GpStatus (WINGDIPAPI *PFNGdipDeletePathIter)(GpPathIterator *iterator);

typedef GpStatus (WINGDIPAPI *PFNGdipPathIterNextSubpath)(GpPathIterator* iterator, INT *resultCount,
        INT* startIndex, INT* endIndex, BOOL* isClosed);

typedef GpStatus (WINGDIPAPI *PFNGdipPathIterNextSubpathPath)(GpPathIterator* iterator, INT* resultCount,
        GpPath* path, BOOL* isClosed);

typedef GpStatus (WINGDIPAPI *PFNGdipPathIterNextPathType)(GpPathIterator* iterator, INT* resultCount,
        BYTE* pathType, INT* startIndex, INT* endIndex);

typedef GpStatus (WINGDIPAPI *PFNGdipPathIterNextMarker)(GpPathIterator* iterator, INT *resultCount,
        INT* startIndex, INT* endIndex);

typedef GpStatus (WINGDIPAPI *PFNGdipPathIterNextMarkerPath)(GpPathIterator* iterator, INT* resultCount,
        GpPath* path);

typedef GpStatus (WINGDIPAPI *PFNGdipPathIterGetCount)(GpPathIterator* iterator, INT* count);

typedef GpStatus (WINGDIPAPI *PFNGdipPathIterGetSubpathCount)(GpPathIterator* iterator, INT* count);

typedef GpStatus (WINGDIPAPI *PFNGdipPathIterIsValid)(GpPathIterator* iterator, BOOL* valid);

typedef GpStatus (WINGDIPAPI *PFNGdipPathIterHasCurve)(GpPathIterator* iterator, BOOL* hasCurve);

typedef GpStatus (WINGDIPAPI *PFNGdipPathIterRewind)(GpPathIterator* iterator);

typedef GpStatus (WINGDIPAPI *PFNGdipPathIterEnumerate)(GpPathIterator* iterator, INT* resultCount,
    GpPointF *points, BYTE *types, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipPathIterCopyData)(GpPathIterator* iterator, INT* resultCount,
    GpPointF* points, BYTE* types, INT startIndex, INT endIndex);

//----------------------------------------------------------------------------
// Matrix APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateMatrix)(GpMatrix **matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateMatrix2)(REAL m11, REAL m12, REAL m21, REAL m22, REAL dx,
                                      REAL dy, GpMatrix **matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateMatrix3)(GDIPCONST GpRectF *rect, GDIPCONST GpPointF *dstplg,
                                      GpMatrix **matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateMatrix3I)(GDIPCONST GpRect *rect, GDIPCONST GpPoint *dstplg,
                                       GpMatrix **matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipCloneMatrix)(GpMatrix *matrix, GpMatrix **cloneMatrix);

typedef GpStatus (WINGDIPAPI *PFNGdipDeleteMatrix)(GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipSetMatrixElements)(GpMatrix *matrix, REAL m11, REAL m12, REAL m21, REAL m22,
                      REAL dx, REAL dy);

typedef GpStatus (WINGDIPAPI *PFNGdipMultiplyMatrix)(GpMatrix *matrix, GpMatrix* matrix2,
                            GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipTranslateMatrix)(GpMatrix *matrix, REAL offsetX, REAL offsetY,
                    GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipScaleMatrix)(GpMatrix *matrix, REAL scaleX, REAL scaleY,
                GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipRotateMatrix)(GpMatrix *matrix, REAL angle, GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipShearMatrix)(GpMatrix *matrix, REAL shearX, REAL shearY,
                GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipInvertMatrix)(GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipTransformMatrixPoints)(GpMatrix *matrix, GpPointF *pts, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipTransformMatrixPointsI)(GpMatrix *matrix, GpPoint *pts, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipVectorTransformMatrixPoints)(GpMatrix *matrix, GpPointF *pts,
                                         INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipVectorTransformMatrixPointsI)(GpMatrix *matrix, GpPoint *pts,
                                         INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetMatrixElements)(GDIPCONST GpMatrix *matrix, REAL *matrixOut);

typedef GpStatus (WINGDIPAPI *PFNGdipIsMatrixInvertible)(GDIPCONST GpMatrix *matrix, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsMatrixIdentity)(GDIPCONST GpMatrix *matrix, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsMatrixEqual)(GDIPCONST GpMatrix *matrix, GDIPCONST GpMatrix *matrix2, 
                  BOOL *result);

//----------------------------------------------------------------------------
// Region APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateRegion)(GpRegion **region);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateRegionRect)(GDIPCONST GpRectF *rect, GpRegion **region);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateRegionRectI)(GDIPCONST GpRect *rect, GpRegion **region);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateRegionPath)(GpPath *path, GpRegion **region);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateRegionRgnData)(GDIPCONST BYTE *regionData, INT size, 
                        GpRegion **region);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateRegionHrgn)(HRGN hRgn, GpRegion **region);

typedef GpStatus (WINGDIPAPI *PFNGdipCloneRegion)(GpRegion *region, GpRegion **cloneRegion);

typedef GpStatus (WINGDIPAPI *PFNGdipDeleteRegion)(GpRegion *region);

typedef GpStatus (WINGDIPAPI *PFNGdipSetInfinite)(GpRegion *region);

typedef GpStatus (WINGDIPAPI *PFNGdipSetEmpty)(GpRegion *region);

typedef GpStatus (WINGDIPAPI *PFNGdipCombineRegionRect)(GpRegion *region, GDIPCONST GpRectF *rect,
                      CombineMode combineMode);

typedef GpStatus (WINGDIPAPI *PFNGdipCombineRegionRectI)(GpRegion *region, GDIPCONST GpRect *rect,
                       CombineMode combineMode);

typedef GpStatus (WINGDIPAPI *PFNGdipCombineRegionPath)(GpRegion *region, GpPath *path, CombineMode combineMode);

typedef GpStatus (WINGDIPAPI *PFNGdipCombineRegionRegion)(GpRegion *region,  GpRegion *region2,
                        CombineMode combineMode);

typedef GpStatus (WINGDIPAPI *PFNGdipTranslateRegion)(GpRegion *region, REAL dx, REAL dy);

typedef GpStatus (WINGDIPAPI *PFNGdipTranslateRegionI)(GpRegion *region, INT dx, INT dy);

typedef GpStatus (WINGDIPAPI *PFNGdipTransformRegion)(GpRegion *region, GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipGetRegionBounds)(GpRegion *region, GpGraphics *graphics,
                             GpRectF *rect);

typedef GpStatus (WINGDIPAPI *PFNGdipGetRegionBoundsI)(GpRegion *region, GpGraphics *graphics,
                             GpRect *rect);

typedef GpStatus (WINGDIPAPI *PFNGdipGetRegionHRgn)(GpRegion *region, GpGraphics *graphics, HRGN *hRgn);

typedef GpStatus (WINGDIPAPI *PFNGdipIsEmptyRegion)(GpRegion *region, GpGraphics *graphics,
                           BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsInfiniteRegion)(GpRegion *region, GpGraphics *graphics,
                              BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsEqualRegion)(GpRegion *region, GpRegion *region2,
                           GpGraphics *graphics, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipGetRegionDataSize)(GpRegion *region, UINT * bufferSize);

typedef GpStatus (WINGDIPAPI *PFNGdipGetRegionData)(GpRegion *region, BYTE * buffer, UINT bufferSize, 
                  UINT * sizeFilled);

typedef GpStatus (WINGDIPAPI *PFNGdipIsVisibleRegionPoint)(GpRegion *region, REAL x, REAL y,
                                  GpGraphics *graphics, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsVisibleRegionPointI)(GpRegion *region, INT x, INT y,
                                  GpGraphics *graphics, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsVisibleRegionRect)(GpRegion *region, REAL x, REAL y, REAL width,
                        REAL height, GpGraphics *graphics, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsVisibleRegionRectI)(GpRegion *region, INT x, INT y, INT width,
                         INT height, GpGraphics *graphics, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipGetRegionScansCount)(GpRegion *region, UINT* count, GpMatrix* matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipGetRegionScans)(GpRegion *region, GpRectF* rects, INT* count, 
                   GpMatrix* matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipGetRegionScansI)(GpRegion *region, GpRect* rects, INT* count, 
                    GpMatrix* matrix);

//----------------------------------------------------------------------------
// Brush APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCloneBrush)(GpBrush *brush, GpBrush **cloneBrush);

typedef GpStatus (WINGDIPAPI *PFNGdipDeleteBrush)(GpBrush *brush);

typedef GpStatus (WINGDIPAPI *PFNGdipGetBrushType)(GpBrush *brush, GpBrushType *type);

//----------------------------------------------------------------------------
// HatchBrush APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateHatchBrush)(GpHatchStyle hatchstyle, ARGB forecol,
                              ARGB backcol, GpHatch **brush);

typedef GpStatus (WINGDIPAPI *PFNGdipGetHatchStyle)(GpHatch *brush, GpHatchStyle *hatchstyle);

typedef GpStatus (WINGDIPAPI *PFNGdipGetHatchForegroundColor)(GpHatch *brush, ARGB* forecol);

typedef GpStatus (WINGDIPAPI *PFNGdipGetHatchBackgroundColor)(GpHatch *brush, ARGB* backcol);

//----------------------------------------------------------------------------
// TextureBrush APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateTexture)(GpImage *image, GpWrapMode wrapmode,
                           GpTexture **texture);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateTexture2)(GpImage *image, GpWrapMode wrapmode, REAL x,
                   REAL y, REAL width, REAL height, GpTexture **texture);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateTextureIA)(GpImage *image, 
                    GDIPCONST GpImageAttributes *imageAttributes,
                    REAL x, REAL y, REAL width, REAL height,
                    GpTexture **texture);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateTexture2I)(GpImage *image, GpWrapMode wrapmode, INT x,
                    INT y, INT width, INT height, GpTexture **texture);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateTextureIAI)(GpImage *image, 
                     GDIPCONST GpImageAttributes *imageAttributes,
                     INT x, INT y, INT width, INT height,
                     GpTexture **texture);


typedef GpStatus (WINGDIPAPI *PFNGdipGetTextureTransform)(GpTexture *brush, GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipSetTextureTransform)(GpTexture *brush, GDIPCONST GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipResetTextureTransform)(GpTexture* brush);

typedef GpStatus (WINGDIPAPI *PFNGdipMultiplyTextureTransform)(GpTexture* brush, GDIPCONST GpMatrix *matrix,
                            GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipTranslateTextureTransform)(GpTexture* brush, REAL dx, REAL dy,
                            GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipScaleTextureTransform)(GpTexture* brush, REAL sx, REAL sy,
                            GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipRotateTextureTransform)(GpTexture* brush, REAL angle, GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipSetTextureWrapMode)(GpTexture *brush, GpWrapMode wrapmode);

typedef GpStatus (WINGDIPAPI *PFNGdipGetTextureWrapMode)(GpTexture *brush, GpWrapMode *wrapmode);

typedef GpStatus (WINGDIPAPI *PFNGdipGetTextureImage)(GpTexture *brush, GpImage **image);

//----------------------------------------------------------------------------
// SolidBrush APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateSolidFill)(ARGB color, GpSolidFill **brush);

typedef GpStatus (WINGDIPAPI *PFNGdipSetSolidFillColor)(GpSolidFill *brush, ARGB color);

typedef GpStatus (WINGDIPAPI *PFNGdipGetSolidFillColor)(GpSolidFill *brush, ARGB *color);

//----------------------------------------------------------------------------
// LineBrush APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateLineBrush)(GDIPCONST GpPointF* point1,
                    GDIPCONST GpPointF* point2,
                    ARGB color1, ARGB color2,
                    GpWrapMode wrapMode,
                    GpLineGradient **lineGradient);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateLineBrushI)(GDIPCONST GpPoint* point1,
                     GDIPCONST GpPoint* point2,
                     ARGB color1, ARGB color2,
                     GpWrapMode wrapMode,
                     GpLineGradient **lineGradient);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateLineBrushFromRect)(GDIPCONST GpRectF* rect,
                            ARGB color1, ARGB color2,
                            LinearGradientMode mode,
                            GpWrapMode wrapMode,
                            GpLineGradient **lineGradient);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateLineBrushFromRectI)(GDIPCONST GpRect* rect,
                             ARGB color1, ARGB color2,
                             LinearGradientMode mode,
                             GpWrapMode wrapMode,
                             GpLineGradient **lineGradient);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateLineBrushFromRectWithAngle)(GDIPCONST GpRectF* rect,
                                     ARGB color1, ARGB color2,
                                     REAL angle,
                                     BOOL isAngleScalable,
                                     GpWrapMode wrapMode,
                                     GpLineGradient **lineGradient);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateLineBrushFromRectWithAngleI)(GDIPCONST GpRect* rect,
                                     ARGB color1, ARGB color2,
                                     REAL angle,
                                     BOOL isAngleScalable,
                                     GpWrapMode wrapMode,
                                     GpLineGradient **lineGradient);

typedef GpStatus (WINGDIPAPI *PFNGdipSetLineColors)(GpLineGradient *brush, ARGB color1, ARGB color2);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLineColors)(GpLineGradient *brush, ARGB* colors);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLineRect)(GpLineGradient *brush, GpRectF *rect);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLineRectI)(GpLineGradient *brush, GpRect *rect);

typedef GpStatus (WINGDIPAPI *PFNGdipSetLineGammaCorrection)(GpLineGradient *brush, BOOL useGammaCorrection);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLineGammaCorrection)(GpLineGradient *brush, BOOL *useGammaCorrection);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLineBlendCount)(GpLineGradient *brush, INT *count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLineBlend)(GpLineGradient *brush, REAL *blend, REAL* positions,
                 INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipSetLineBlend)(GpLineGradient *brush, GDIPCONST REAL *blend,
                 GDIPCONST REAL* positions, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLinePresetBlendCount)(GpLineGradient *brush, INT *count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLinePresetBlend)(GpLineGradient *brush, ARGB *blend,
                                           REAL* positions, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipSetLinePresetBlend)(GpLineGradient *brush, GDIPCONST ARGB *blend,
                       GDIPCONST REAL* positions, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipSetLineSigmaBlend)(GpLineGradient *brush, REAL focus, REAL scale);

typedef GpStatus (WINGDIPAPI *PFNGdipSetLineLinearBlend)(GpLineGradient *brush, REAL focus, REAL scale);

typedef GpStatus (WINGDIPAPI *PFNGdipSetLineWrapMode)(GpLineGradient *brush, GpWrapMode wrapmode);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLineWrapMode)(GpLineGradient *brush, GpWrapMode *wrapmode);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLineTransform)(GpLineGradient *brush, GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipSetLineTransform)(GpLineGradient *brush, GDIPCONST GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipResetLineTransform)(GpLineGradient* brush);

typedef GpStatus (WINGDIPAPI *PFNGdipMultiplyLineTransform)(GpLineGradient* brush, GDIPCONST GpMatrix *matrix,
                            GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipTranslateLineTransform)(GpLineGradient* brush, REAL dx, REAL dy,
                            GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipScaleLineTransform)(GpLineGradient* brush, REAL sx, REAL sy,
                            GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipRotateLineTransform)(GpLineGradient* brush, REAL angle, 
                        GpMatrixOrder order);

//----------------------------------------------------------------------------
// PathGradientBrush APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreatePathGradient)(GDIPCONST GpPointF* points,
                                    INT count,
                                    GpWrapMode wrapMode,
                                    GpPathGradient **polyGradient);

typedef GpStatus (WINGDIPAPI *PFNGdipCreatePathGradientI)(GDIPCONST GpPoint* points,
                                    INT count,
                                    GpWrapMode wrapMode,
                                    GpPathGradient **polyGradient);

typedef GpStatus (WINGDIPAPI *PFNGdipCreatePathGradientFromPath)(GDIPCONST GpPath* path,
                                    GpPathGradient **polyGradient);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientCenterColor)(
                        GpPathGradient *brush, ARGB* colors);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientCenterColor)(
                        GpPathGradient *brush, ARGB colors);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientSurroundColorsWithCount)(
                        GpPathGradient *brush, ARGB* color, INT* count);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientSurroundColorsWithCount)(
                        GpPathGradient *brush,
                        GDIPCONST ARGB* color, INT* count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientPath)(GpPathGradient *brush, GpPath *path);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientPath)(GpPathGradient *brush, GDIPCONST GpPath *path);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientCenterPoint)(
                        GpPathGradient *brush, GpPointF* points);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientCenterPointI)(
                        GpPathGradient *brush, GpPoint* points);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientCenterPoint)(
                        GpPathGradient *brush, GDIPCONST GpPointF* points);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientCenterPointI)(
                        GpPathGradient *brush, GDIPCONST GpPoint* points);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientRect)(GpPathGradient *brush, GpRectF *rect);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientRectI)(GpPathGradient *brush, GpRect *rect);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientPointCount)(GpPathGradient *brush, INT* count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientSurroundColorCount)(GpPathGradient *brush, INT* count);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientGammaCorrection)(GpPathGradient *brush, 
                                   BOOL useGammaCorrection);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientGammaCorrection)(GpPathGradient *brush, 
                                   BOOL *useGammaCorrection);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientBlendCount)(GpPathGradient *brush,
                                             INT *count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientBlend)(GpPathGradient *brush,
                                    REAL *blend, REAL *positions, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientBlend)(GpPathGradient *brush,
                GDIPCONST REAL *blend, GDIPCONST REAL *positions, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientPresetBlendCount)(GpPathGradient *brush, INT *count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientPresetBlend)(GpPathGradient *brush, ARGB *blend,
                                                REAL* positions, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientPresetBlend)(GpPathGradient *brush, GDIPCONST ARGB *blend,
                                        GDIPCONST REAL* positions, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientSigmaBlend)(GpPathGradient *brush, REAL focus, REAL scale);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientLinearBlend)(GpPathGradient *brush, REAL focus, REAL scale);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientWrapMode)(GpPathGradient *brush,
                                         GpWrapMode *wrapmode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientWrapMode)(GpPathGradient *brush,
                                         GpWrapMode wrapmode);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientTransform)(GpPathGradient *brush,
                                          GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientTransform)(GpPathGradient *brush,
                                          GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipResetPathGradientTransform)(GpPathGradient* brush);

typedef GpStatus (WINGDIPAPI *PFNGdipMultiplyPathGradientTransform)(GpPathGradient* brush, 
                                  GDIPCONST GpMatrix *matrix,
                                  GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipTranslatePathGradientTransform)(GpPathGradient* brush, REAL dx, REAL dy,
                                   GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipScalePathGradientTransform)(GpPathGradient* brush, REAL sx, REAL sy,
                               GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipRotatePathGradientTransform)(GpPathGradient* brush, REAL angle,
                                GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPathGradientFocusScales)(GpPathGradient *brush, REAL* xScale, 
                               REAL* yScale);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPathGradientFocusScales)(GpPathGradient *brush, REAL xScale, 
                               REAL yScale);

//----------------------------------------------------------------------------
// Pen APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreatePen1)(ARGB color, REAL width, GpUnit unit, GpPen **pen);

typedef GpStatus (WINGDIPAPI *PFNGdipCreatePen2)(GpBrush *brush, REAL width, GpUnit unit,
                        GpPen **pen);

typedef GpStatus (WINGDIPAPI *PFNGdipClonePen)(GpPen *pen, GpPen **clonepen);

typedef GpStatus (WINGDIPAPI *PFNGdipDeletePen)(GpPen *pen);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenWidth)(GpPen *pen, REAL width);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenWidth)(GpPen *pen, REAL *width);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenUnit)(GpPen *pen, GpUnit unit);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenUnit)(GpPen *pen, GpUnit *unit);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenLineCap197819)(GpPen *pen, GpLineCap startCap, GpLineCap endCap,
                  GpDashCap dashCap);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenStartCap)(GpPen *pen, GpLineCap startCap);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenEndCap)(GpPen *pen, GpLineCap endCap);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenDashCap197819)(GpPen *pen, GpDashCap dashCap);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenStartCap)(GpPen *pen, GpLineCap *startCap);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenEndCap)(GpPen *pen, GpLineCap *endCap);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenDashCap197819)(GpPen *pen, GpDashCap *dashCap);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenLineJoin)(GpPen *pen, GpLineJoin lineJoin);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenLineJoin)(GpPen *pen, GpLineJoin *lineJoin);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenCustomStartCap)(GpPen *pen, GpCustomLineCap* customCap);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenCustomStartCap)(GpPen *pen, GpCustomLineCap** customCap);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenCustomEndCap)(GpPen *pen, GpCustomLineCap* customCap);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenCustomEndCap)(GpPen *pen, GpCustomLineCap** customCap);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenMiterLimit)(GpPen *pen, REAL miterLimit);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenMiterLimit)(GpPen *pen, REAL *miterLimit);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenMode)(GpPen *pen, GpPenAlignment penMode);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenMode)(GpPen *pen, GpPenAlignment *penMode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenTransform)(GpPen *pen, GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenTransform)(GpPen *pen, GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipResetPenTransform)(GpPen *pen);

typedef GpStatus (WINGDIPAPI *PFNGdipMultiplyPenTransform)(GpPen *pen, GDIPCONST GpMatrix *matrix,
                           GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipTranslatePenTransform)(GpPen *pen, REAL dx, REAL dy,
                            GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipScalePenTransform)(GpPen *pen, REAL sx, REAL sy,
                            GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipRotatePenTransform)(GpPen *pen, REAL angle, GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenColor)(GpPen *pen, ARGB argb);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenColor)(GpPen *pen, ARGB *argb);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenBrushFill)(GpPen *pen, GpBrush *brush);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenBrushFill)(GpPen *pen, GpBrush **brush);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenFillType)(GpPen *pen, GpPenType* type);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenDashStyle)(GpPen *pen, GpDashStyle *dashstyle);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenDashStyle)(GpPen *pen, GpDashStyle dashstyle);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenDashOffset)(GpPen *pen, REAL *offset);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenDashOffset)(GpPen *pen, REAL offset);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenDashCount)(GpPen *pen, INT *count);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenDashArray)(GpPen *pen, GDIPCONST REAL *dash, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenDashArray)(GpPen *pen, REAL *dash, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenCompoundCount)(GpPen *pen, INT *count);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPenCompoundArray)(GpPen *pen, GDIPCONST REAL *dash, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPenCompoundArray)(GpPen *pen, REAL *dash, INT count);

//----------------------------------------------------------------------------
// CustomLineCap APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateCustomLineCap)(GpPath* fillPath, GpPath* strokePath,
   GpLineCap baseCap, REAL baseInset, GpCustomLineCap **customCap);

typedef GpStatus (WINGDIPAPI *PFNGdipDeleteCustomLineCap)(GpCustomLineCap* customCap);

typedef GpStatus (WINGDIPAPI *PFNGdipCloneCustomLineCap)(GpCustomLineCap* customCap,
                       GpCustomLineCap** clonedCap);

typedef GpStatus (WINGDIPAPI *PFNGdipGetCustomLineCapType)(GpCustomLineCap* customCap,
                       CustomLineCapType* capType);

typedef GpStatus (WINGDIPAPI *PFNGdipSetCustomLineCapStrokeCaps)(GpCustomLineCap* customCap,
                               GpLineCap startCap, GpLineCap endCap);

typedef GpStatus (WINGDIPAPI *PFNGdipGetCustomLineCapStrokeCaps)(GpCustomLineCap* customCap,
                               GpLineCap* startCap, GpLineCap* endCap);

typedef GpStatus (WINGDIPAPI *PFNGdipSetCustomLineCapStrokeJoin)(GpCustomLineCap* customCap, 
                               GpLineJoin lineJoin);

typedef GpStatus (WINGDIPAPI *PFNGdipGetCustomLineCapStrokeJoin)(GpCustomLineCap* customCap, 
                               GpLineJoin* lineJoin);

typedef GpStatus (WINGDIPAPI *PFNGdipSetCustomLineCapBaseCap)(GpCustomLineCap* customCap, GpLineCap baseCap);

typedef GpStatus (WINGDIPAPI *PFNGdipGetCustomLineCapBaseCap)(GpCustomLineCap* customCap, GpLineCap* baseCap);

typedef GpStatus (WINGDIPAPI *PFNGdipSetCustomLineCapBaseInset)(GpCustomLineCap* customCap, REAL inset);

typedef GpStatus (WINGDIPAPI *PFNGdipGetCustomLineCapBaseInset)(GpCustomLineCap* customCap, REAL* inset);

typedef GpStatus (WINGDIPAPI *PFNGdipSetCustomLineCapWidthScale)(GpCustomLineCap* customCap, REAL widthScale);

typedef GpStatus (WINGDIPAPI *PFNGdipGetCustomLineCapWidthScale)(GpCustomLineCap* customCap, REAL* widthScale);

//----------------------------------------------------------------------------
// AdjustableArrowCap APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateAdjustableArrowCap)(REAL height, REAL width, BOOL isFilled,
                             GpAdjustableArrowCap **cap);

typedef GpStatus (WINGDIPAPI *PFNGdipSetAdjustableArrowCapHeight)(GpAdjustableArrowCap* cap, REAL height);

typedef GpStatus (WINGDIPAPI *PFNGdipGetAdjustableArrowCapHeight)(GpAdjustableArrowCap* cap, REAL* height);

typedef GpStatus (WINGDIPAPI *PFNGdipSetAdjustableArrowCapWidth)(GpAdjustableArrowCap* cap, REAL width);

typedef GpStatus (WINGDIPAPI *PFNGdipGetAdjustableArrowCapWidth)(GpAdjustableArrowCap* cap, REAL* width);

typedef GpStatus (WINGDIPAPI *PFNGdipSetAdjustableArrowCapMiddleInset)(GpAdjustableArrowCap* cap, 
                                     REAL middleInset);

typedef GpStatus (WINGDIPAPI *PFNGdipGetAdjustableArrowCapMiddleInset)(GpAdjustableArrowCap* cap, 
                                     REAL* middleInset);

typedef GpStatus (WINGDIPAPI *PFNGdipSetAdjustableArrowCapFillState)(GpAdjustableArrowCap* cap, BOOL fillState);

typedef GpStatus (WINGDIPAPI *PFNGdipGetAdjustableArrowCapFillState)(GpAdjustableArrowCap* cap, BOOL* fillState);

//----------------------------------------------------------------------------
// Image APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipLoadImageFromStream)(IStream* stream, GpImage **image);

typedef GpStatus (WINGDIPAPI *PFNGdipLoadImageFromFile)(GDIPCONST WCHAR* filename, GpImage **image);

typedef GpStatus (WINGDIPAPI *PFNGdipLoadImageFromStreamICM)(IStream* stream, GpImage **image);

typedef GpStatus (WINGDIPAPI *PFNGdipLoadImageFromFileICM)(GDIPCONST WCHAR* filename, GpImage **image);

typedef GpStatus (WINGDIPAPI *PFNGdipCloneImage)(GpImage *image, GpImage **cloneImage);

typedef GpStatus (WINGDIPAPI *PFNGdipDisposeImage)(GpImage *image);

typedef GpStatus (WINGDIPAPI *PFNGdipSaveImageToFile)(GpImage *image, GDIPCONST WCHAR* filename,
                    GDIPCONST CLSID* clsidEncoder, 
                    GDIPCONST EncoderParameters* encoderParams);

typedef GpStatus (WINGDIPAPI *PFNGdipSaveImageToStream)(GpImage *image, IStream* stream,
                      GDIPCONST CLSID* clsidEncoder, 
                      GDIPCONST EncoderParameters* encoderParams);

typedef GpStatus (WINGDIPAPI *PFNGdipSaveAdd)(GpImage *image, GDIPCONST EncoderParameters* encoderParams);

typedef GpStatus (WINGDIPAPI *PFNGdipSaveAddImage)(GpImage *image, GpImage* newImage,
                 GDIPCONST EncoderParameters* encoderParams);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageGraphicsContext)(GpImage *image, GpGraphics **graphics);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageBounds)(GpImage *image, GpRectF *srcRect, GpUnit *srcUnit);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageDimension)(GpImage *image, REAL *width, REAL *height);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageType)(GpImage *image, ImageType *type);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageWidth)(GpImage *image, UINT *width);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageHeight)(GpImage *image, UINT *height);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageHorizontalResolution)(GpImage *image, REAL *resolution);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageVerticalResolution)(GpImage *image, REAL *resolution);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageFlags)(GpImage *image, UINT *flags);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageRawFormat)(GpImage *image, GUID *format);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImagePixelFormat)(GpImage *image, PixelFormat *format);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageThumbnail)(GpImage *image, UINT thumbWidth, UINT thumbHeight,
                      GpImage **thumbImage,
                      GetThumbnailImageAbort callback, VOID * callbackData);

typedef GpStatus (WINGDIPAPI *PFNGdipGetEncoderParameterListSize)(GpImage *image, GDIPCONST CLSID* clsidEncoder,
                                UINT* size);

typedef GpStatus (WINGDIPAPI *PFNGdipGetEncoderParameterList)(GpImage *image, GDIPCONST CLSID* clsidEncoder,
                            UINT size, EncoderParameters* buffer);

typedef GpStatus (WINGDIPAPI *PFNGdipImageGetFrameDimensionsCount)(GpImage* image, UINT* count);

typedef GpStatus (WINGDIPAPI *PFNGdipImageGetFrameDimensionsList)(GpImage* image, GUID* dimensionIDs, 
                                UINT count);

typedef GpStatus (WINGDIPAPI *PFNGdipImageGetFrameCount)(GpImage *image, GDIPCONST GUID* dimensionID, 
                       UINT* count);

typedef GpStatus (WINGDIPAPI *PFNGdipImageSelectActiveFrame)(GpImage *image, GDIPCONST GUID* dimensionID,
                           UINT frameIndex);

typedef GpStatus (WINGDIPAPI *PFNGdipImageRotateFlip)(GpImage *image, RotateFlipType rfType);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImagePalette)(GpImage *image, ColorPalette *palette, INT size);

typedef GpStatus (WINGDIPAPI *PFNGdipSetImagePalette)(GpImage *image, GDIPCONST ColorPalette *palette);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImagePaletteSize)(GpImage *image, INT *size);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPropertyCount)(GpImage *image, UINT* numOfProperty);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPropertyIdList)(GpImage *image, UINT numOfProperty, PROPID* list);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPropertyItemSize)(GpImage *image, PROPID propId, UINT* size);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPropertyItem)(GpImage *image, PROPID propId,UINT propSize,
                    PropertyItem* buffer);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPropertySize)(GpImage *image, UINT* totalBufferSize, 
                    UINT* numProperties);

typedef GpStatus (WINGDIPAPI *PFNGdipGetAllPropertyItems)(GpImage *image, UINT totalBufferSize,
                        UINT numProperties, PropertyItem* allItems);

typedef GpStatus (WINGDIPAPI *PFNGdipRemovePropertyItem)(GpImage *image, PROPID propId);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPropertyItem)(GpImage *image, GDIPCONST PropertyItem* item);

typedef GpStatus (WINGDIPAPI *PFNGdipImageForceValidation)(GpImage *image);

//----------------------------------------------------------------------------
// Bitmap APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateBitmapFromStream)(IStream* stream, GpBitmap **bitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateBitmapFromFile)(GDIPCONST WCHAR* filename, GpBitmap **bitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateBitmapFromStreamICM)(IStream* stream, GpBitmap **bitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateBitmapFromFileICM)(GDIPCONST WCHAR* filename, GpBitmap **bitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateBitmapFromScan0)(INT width,
                          INT height,
                          INT stride,
                          PixelFormat format,
                          BYTE* scan0,
                          GpBitmap** bitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateBitmapFromGraphics)(INT width,
                             INT height,
                             GpGraphics* target,
                             GpBitmap** bitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateBitmapFromDirectDrawSurface)(IDirectDrawSurface7* surface,
                                      GpBitmap** bitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateBitmapFromGdiDib)(GDIPCONST BITMAPINFO* gdiBitmapInfo,
                           VOID* gdiBitmapData,
                           GpBitmap** bitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateBitmapFromHBITMAP)(HBITMAP hbm,
                            HPALETTE hpal,
                            GpBitmap** bitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateHBITMAPFromBitmap)(GpBitmap* bitmap,
                            HBITMAP* hbmReturn,
                            ARGB background);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateBitmapFromHICON)(HICON hicon,
                          GpBitmap** bitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateHICONFromBitmap)(GpBitmap* bitmap,
                          HICON* hbmReturn);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateBitmapFromResource)(HINSTANCE hInstance,
                             GDIPCONST WCHAR* lpBitmapName,
                             GpBitmap** bitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipCloneBitmapArea)(REAL x, REAL y, REAL width, REAL height,
                            PixelFormat format,
                            GpBitmap *srcBitmap,
                            GpBitmap **dstBitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipCloneBitmapAreaI)(INT x,
                     INT y,
                     INT width,
                     INT height,
                     PixelFormat format,
                     GpBitmap *srcBitmap,
                     GpBitmap **dstBitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipBitmapLockBits)(GpBitmap* bitmap,
                   GDIPCONST GpRect* rect,
                   UINT flags,
                   PixelFormat format,
                   BitmapData* lockedBitmapData);

typedef GpStatus (WINGDIPAPI *PFNGdipBitmapUnlockBits)(GpBitmap* bitmap,
                     BitmapData* lockedBitmapData);

typedef GpStatus (WINGDIPAPI *PFNGdipBitmapGetPixel)(GpBitmap* bitmap, INT x, INT y, ARGB *color);

typedef GpStatus (WINGDIPAPI *PFNGdipBitmapSetPixel)(GpBitmap* bitmap, INT x, INT y, ARGB color);

typedef GpStatus (WINGDIPAPI *PFNGdipBitmapSetResolution)(GpBitmap* bitmap, REAL xdpi, REAL ydpi);

//----------------------------------------------------------------------------
// ImageAttributes APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateImageAttributes)(GpImageAttributes **imageattr);

typedef GpStatus (WINGDIPAPI *PFNGdipCloneImageAttributes)(GDIPCONST GpImageAttributes *imageattr,
                         GpImageAttributes **cloneImageattr);

typedef GpStatus (WINGDIPAPI *PFNGdipDisposeImageAttributes)(GpImageAttributes *imageattr);

typedef GpStatus (WINGDIPAPI *PFNGdipSetImageAttributesToIdentity)(GpImageAttributes *imageattr,
                                 ColorAdjustType type);
typedef GpStatus (WINGDIPAPI *PFNGdipResetImageAttributes)(GpImageAttributes *imageattr,
                         ColorAdjustType type);

typedef GpStatus (WINGDIPAPI *PFNGdipSetImageAttributesColorMatrix)(GpImageAttributes *imageattr,
                               ColorAdjustType type,
                               BOOL enableFlag,
                               GDIPCONST ColorMatrix* colorMatrix,
                               GDIPCONST ColorMatrix* grayMatrix,
                               ColorMatrixFlags flags);

typedef GpStatus (WINGDIPAPI *PFNGdipSetImageAttributesThreshold)(GpImageAttributes *imageattr,
                                ColorAdjustType type,
                                BOOL enableFlag,
                                REAL threshold);

typedef GpStatus (WINGDIPAPI *PFNGdipSetImageAttributesGamma)(GpImageAttributes *imageattr,
                            ColorAdjustType type,
                            BOOL enableFlag,
                            REAL gamma);

typedef GpStatus (WINGDIPAPI *PFNGdipSetImageAttributesNoOp)(GpImageAttributes *imageattr,
                           ColorAdjustType type,
                           BOOL enableFlag);

typedef GpStatus (WINGDIPAPI *PFNGdipSetImageAttributesColorKeys)(GpImageAttributes *imageattr,
                                ColorAdjustType type,
                                BOOL enableFlag,
                                ARGB colorLow,
                                ARGB colorHigh);

typedef GpStatus (WINGDIPAPI *PFNGdipSetImageAttributesOutputChannel)(GpImageAttributes *imageattr,
                                    ColorAdjustType type,
                                    BOOL enableFlag,
                                    ColorChannelFlags channelFlags);

typedef GpStatus (WINGDIPAPI *PFNGdipSetImageAttributesOutputChannelColorProfile)(GpImageAttributes *imageattr,
                                                ColorAdjustType type,
                                                BOOL enableFlag,
                                                GDIPCONST 
                                                  WCHAR *colorProfileFilename);

typedef GpStatus (WINGDIPAPI *PFNGdipSetImageAttributesRemapTable)(GpImageAttributes *imageattr,
                                 ColorAdjustType type,
                                 BOOL enableFlag,
                                 UINT mapSize,
                                 GDIPCONST ColorMap *map);
typedef GpStatus (WINGDIPAPI *PFNGdipSetImageAttributesWrapMode)(
    GpImageAttributes *imageAttr,
    WrapMode wrap,
    ARGB argb,
    BOOL clamp
);

typedef GpStatus (WINGDIPAPI *PFNGdipSetImageAttributesICMMode)(
    GpImageAttributes *imageAttr,
    BOOL on
);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageAttributesAdjustedPalette)(
    GpImageAttributes *imageAttr,
    ColorPalette * colorPalette,
    ColorAdjustType colorAdjustType
);

//----------------------------------------------------------------------------
// Graphics APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipFlush)(GpGraphics *graphics, GpFlushIntention intention);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateFromHDC)(HDC hdc, GpGraphics **graphics);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateFromHDC2)(HDC hdc, HANDLE hDevice, GpGraphics **graphics);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateFromHWND)(HWND hwnd, GpGraphics **graphics);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateFromHWNDICM)(HWND hwnd, GpGraphics **graphics);

typedef GpStatus (WINGDIPAPI *PFNGdipDeleteGraphics)(GpGraphics *graphics);

typedef GpStatus (WINGDIPAPI *PFNGdipGetDC)(GpGraphics* graphics, HDC * hdc);

typedef GpStatus (WINGDIPAPI *PFNGdipReleaseDC)(GpGraphics* graphics, HDC hdc);

typedef GpStatus (WINGDIPAPI *PFNGdipSetCompositingMode)(GpGraphics *graphics, CompositingMode compositingMode);

typedef GpStatus (WINGDIPAPI *PFNGdipGetCompositingMode)(GpGraphics *graphics, CompositingMode *compositingMode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetRenderingOrigin)(GpGraphics *graphics, INT x, INT y);

typedef GpStatus (WINGDIPAPI *PFNGdipGetRenderingOrigin)(GpGraphics *graphics, INT *x, INT *y);

typedef GpStatus (WINGDIPAPI *PFNGdipSetCompositingQuality)(GpGraphics *graphics, 
                          CompositingQuality compositingQuality);

typedef GpStatus (WINGDIPAPI *PFNGdipGetCompositingQuality)(GpGraphics *graphics, 
                          CompositingQuality *compositingQuality);
                          
typedef GpStatus (WINGDIPAPI *PFNGdipSetSmoothingMode)(GpGraphics *graphics, SmoothingMode smoothingMode);

typedef GpStatus (WINGDIPAPI *PFNGdipGetSmoothingMode)(GpGraphics *graphics, SmoothingMode *smoothingMode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPixelOffsetMode)(GpGraphics* graphics, PixelOffsetMode pixelOffsetMode);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPixelOffsetMode)(GpGraphics *graphics, PixelOffsetMode *pixelOffsetMode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetTextRenderingHint)(GpGraphics *graphics, TextRenderingHint mode);

typedef GpStatus (WINGDIPAPI *PFNGdipGetTextRenderingHint)(GpGraphics *graphics, TextRenderingHint *mode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetTextContrast)(GpGraphics *graphics, UINT contrast);

typedef GpStatus (WINGDIPAPI *PFNGdipGetTextContrast)(GpGraphics *graphics, UINT * contrast);

typedef GpStatus (WINGDIPAPI *PFNGdipSetInterpolationMode)(GpGraphics *graphics, 
                         InterpolationMode interpolationMode);

typedef GpStatus (WINGDIPAPI *PFNGdipGetInterpolationMode)(GpGraphics *graphics, 
                         InterpolationMode *interpolationMode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetWorldTransform)(GpGraphics *graphics, GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipResetWorldTransform)(GpGraphics *graphics);

typedef GpStatus (WINGDIPAPI *PFNGdipMultiplyWorldTransform)(GpGraphics *graphics, GDIPCONST GpMatrix *matrix,
                           GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipTranslateWorldTransform)(GpGraphics *graphics, REAL dx, REAL dy,
                            GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipScaleWorldTransform)(GpGraphics *graphics, REAL sx, REAL sy,
                        GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipRotateWorldTransform)(GpGraphics *graphics, REAL angle, 
                         GpMatrixOrder order);

typedef GpStatus (WINGDIPAPI *PFNGdipGetWorldTransform)(GpGraphics *graphics, GpMatrix *matrix);

typedef GpStatus (WINGDIPAPI *PFNGdipResetPageTransform)(GpGraphics *graphics);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPageUnit)(GpGraphics *graphics, GpUnit *unit);

typedef GpStatus (WINGDIPAPI *PFNGdipGetPageScale)(GpGraphics *graphics, REAL *scale);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPageUnit)(GpGraphics *graphics, GpUnit unit);

typedef GpStatus (WINGDIPAPI *PFNGdipSetPageScale)(GpGraphics *graphics, REAL scale);

typedef GpStatus (WINGDIPAPI *PFNGdipGetDpiX)(GpGraphics *graphics, REAL* dpi);

typedef GpStatus (WINGDIPAPI *PFNGdipGetDpiY)(GpGraphics *graphics, REAL* dpi);

typedef GpStatus (WINGDIPAPI *PFNGdipTransformPoints)(GpGraphics *graphics, GpCoordinateSpace destSpace,
                             GpCoordinateSpace srcSpace, GpPointF *points,
                             INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipTransformPointsI)(GpGraphics *graphics, GpCoordinateSpace destSpace,
                             GpCoordinateSpace srcSpace, GpPoint *points,
                             INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipGetNearestColor)(GpGraphics *graphics, ARGB* argb);

// Creates the Win9x Halftone Palette(even on NT) with correct Desktop colors
typedef HPALETTE(WINGDIPAPI *PFNGdipCreateHalftonePalette)();

typedef GpStatus (WINGDIPAPI *PFNGdipDrawLine)(GpGraphics *graphics, GpPen *pen, REAL x1, REAL y1,
                      REAL x2, REAL y2);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawLineI)(GpGraphics *graphics, GpPen *pen, INT x1, INT y1,
                      INT x2, INT y2);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawLines)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points,
                       INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawLinesI)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points,
                       INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawArc)(GpGraphics *graphics, GpPen *pen, REAL x, REAL y,
            REAL width, REAL height, REAL startAngle, REAL sweepAngle);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawArcI)(GpGraphics *graphics, GpPen *pen, INT x, INT y,
                     INT width, INT height, REAL startAngle, REAL sweepAngle);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawBezier)(GpGraphics *graphics, GpPen *pen, REAL x1, REAL y1,
                        REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawBezierI)(GpGraphics *graphics, GpPen *pen, INT x1, INT y1,
                        INT x2, INT y2, INT x3, INT y3, INT x4, INT y4);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawBeziers)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points,
                         INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawBeziersI)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points,
                         INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawRectangle)(GpGraphics *graphics, GpPen *pen, REAL x, REAL y,
                      REAL width, REAL height);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawRectangleI)(GpGraphics *graphics, GpPen *pen, INT x, INT y,
                      INT width, INT height);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawRectangles)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpRectF *rects,
                       INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawRectanglesI)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpRect *rects,
                       INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawEllipse)(GpGraphics *graphics, GpPen *pen, REAL x, REAL y,
                         REAL width, REAL height);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawEllipseI)(GpGraphics *graphics, GpPen *pen, INT x, INT y,
                         INT width, INT height);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawPie)(GpGraphics *graphics, GpPen *pen, REAL x, REAL y,
                     REAL width, REAL height, REAL startAngle, 
            REAL sweepAngle);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawPieI)(GpGraphics *graphics, GpPen *pen, INT x, INT y,
                     INT width, INT height, REAL startAngle, REAL sweepAngle);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawPolygon)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points,
                         INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawPolygonI)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points,
                         INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawPath)(GpGraphics *graphics, GpPen *pen, GpPath *path);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawCurve)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points,
                       INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawCurveI)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points,
                       INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawCurve2)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points,
                       INT count, REAL tension);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawCurve2I)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points,
                       INT count, REAL tension);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawCurve3)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points,
               INT count, INT offset, INT numberOfSegments, REAL tension);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawCurve3I)(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPoint *points,
                INT count, INT offset, INT numberOfSegments, REAL tension);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawClosedCurve)(GpGraphics *graphics, GpPen *pen,
                    GDIPCONST GpPointF *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawClosedCurveI)(GpGraphics *graphics, GpPen *pen,
                     GDIPCONST GpPoint *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawClosedCurve2)(GpGraphics *graphics, GpPen *pen,
                     GDIPCONST GpPointF *points, INT count, REAL tension);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawClosedCurve2I)(GpGraphics *graphics, GpPen *pen,
                      GDIPCONST GpPoint *points, INT count, REAL tension);

typedef GpStatus (WINGDIPAPI *PFNGdipGraphicsClear)(GpGraphics *graphics, ARGB color);

typedef GpStatus (WINGDIPAPI *PFNGdipFillRectangle)(GpGraphics *graphics, GpBrush *brush, REAL x, REAL y,
                  REAL width, REAL height);

typedef GpStatus (WINGDIPAPI *PFNGdipFillRectangleI)(GpGraphics *graphics, GpBrush *brush, INT x, INT y,
                   INT width, INT height);

typedef GpStatus (WINGDIPAPI *PFNGdipFillRectangles)(GpGraphics *graphics, GpBrush *brush,
                   GDIPCONST GpRectF *rects, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipFillRectanglesI)(GpGraphics *graphics, GpBrush *brush,
                    GDIPCONST GpRect *rects, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipFillPolygon)(GpGraphics *graphics, GpBrush *brush,
                GDIPCONST GpPointF *points, INT count, GpFillMode fillMode);

typedef GpStatus (WINGDIPAPI *PFNGdipFillPolygonI)(GpGraphics *graphics, GpBrush *brush,
                 GDIPCONST GpPoint *points, INT count, GpFillMode fillMode);

typedef GpStatus (WINGDIPAPI *PFNGdipFillPolygon2)(GpGraphics *graphics, GpBrush *brush,
                 GDIPCONST GpPointF *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipFillPolygon2I)(GpGraphics *graphics, GpBrush *brush,
                  GDIPCONST GpPoint *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipFillEllipse)(GpGraphics *graphics, GpBrush *brush, REAL x, REAL y,
                REAL width, REAL height);

typedef GpStatus (WINGDIPAPI *PFNGdipFillEllipseI)(GpGraphics *graphics, GpBrush *brush, INT x, INT y,
                 INT width, INT height);

typedef GpStatus (WINGDIPAPI *PFNGdipFillPie)(GpGraphics *graphics, GpBrush *brush, REAL x, REAL y,
            REAL width, REAL height, REAL startAngle, REAL sweepAngle);

typedef GpStatus (WINGDIPAPI *PFNGdipFillPieI)(GpGraphics *graphics, GpBrush *brush, INT x, INT y,
             INT width, INT height, REAL startAngle, REAL sweepAngle);

typedef GpStatus (WINGDIPAPI *PFNGdipFillPath)(GpGraphics *graphics, GpBrush *brush, GpPath *path);

typedef GpStatus (WINGDIPAPI *PFNGdipFillClosedCurve)(GpGraphics *graphics, GpBrush *brush,
                              GDIPCONST GpPointF *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipFillClosedCurveI)(GpGraphics *graphics, GpBrush *brush,
                              GDIPCONST GpPoint *points, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipFillClosedCurve2)(GpGraphics *graphics, GpBrush *brush,
                              GDIPCONST GpPointF *points, INT count,
                              REAL tension, GpFillMode fillMode);

typedef GpStatus (WINGDIPAPI *PFNGdipFillClosedCurve2I)(GpGraphics *graphics, GpBrush *brush,
                              GDIPCONST GpPoint *points, INT count,
                              REAL tension, GpFillMode fillMode);

typedef GpStatus (WINGDIPAPI *PFNGdipFillRegion)(GpGraphics *graphics, GpBrush *brush,
                        GpRegion *region);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawImage)(GpGraphics *graphics, GpImage *image, REAL x, REAL y);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawImageI)(GpGraphics *graphics, GpImage *image, INT x, INT y);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawImageRect)(GpGraphics *graphics, GpImage *image, REAL x, REAL y,
                           REAL width, REAL height);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawImageRectI)(GpGraphics *graphics, GpImage *image, INT x, INT y,
                           INT width, INT height);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawImagePoints)(GpGraphics *graphics, GpImage *image,
                             GDIPCONST GpPointF *dstpoints, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawImagePointsI)(GpGraphics *graphics, GpImage *image,
                             GDIPCONST GpPoint *dstpoints, INT count);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawImagePointRect)(GpGraphics *graphics, GpImage *image, REAL x,
                                REAL y, REAL srcx, REAL srcy, REAL srcwidth,
                                REAL srcheight, GpUnit srcUnit);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawImagePointRectI)(GpGraphics *graphics, GpImage *image, INT x,
                                INT y, INT srcx, INT srcy, INT srcwidth,
                                INT srcheight, GpUnit srcUnit);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawImageRectRect)(GpGraphics *graphics, GpImage *image, REAL dstx,
                      REAL dsty, REAL dstwidth, REAL dstheight,
                      REAL srcx, REAL srcy, REAL srcwidth, REAL srcheight,
                      GpUnit srcUnit,
                      GDIPCONST GpImageAttributes* imageAttributes,
                      DrawImageAbort callback, VOID * callbackData);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawImageRectRectI)(GpGraphics *graphics, GpImage *image, INT dstx,
                       INT dsty, INT dstwidth, INT dstheight,
                       INT srcx, INT srcy, INT srcwidth, INT srcheight,
                       GpUnit srcUnit,
                       GDIPCONST GpImageAttributes* imageAttributes,
                       DrawImageAbort callback, VOID * callbackData);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawImagePointsRect)(GpGraphics *graphics, GpImage *image,
                        GDIPCONST GpPointF *points, INT count, REAL srcx,
                        REAL srcy, REAL srcwidth, REAL srcheight,
                        GpUnit srcUnit,
                        GDIPCONST GpImageAttributes* imageAttributes,
                        DrawImageAbort callback, VOID * callbackData);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawImagePointsRectI)(GpGraphics *graphics, GpImage *image,
                         GDIPCONST GpPoint *points, INT count, INT srcx,
                         INT srcy, INT srcwidth, INT srcheight,
                         GpUnit srcUnit,
                         GDIPCONST GpImageAttributes* imageAttributes,
                         DrawImageAbort callback, VOID * callbackData);

typedef GpStatus (WINGDIPAPI *PFNGdipEnumerateMetafileDestPoint)(
    GpGraphics *            graphics,
    GDIPCONST GpMetafile *  metafile,
    GDIPCONST PointF &      destPoint,
    EnumerateMetafileProc   callback,
    VOID *                  callbackData,
    GDIPCONST GpImageAttributes *     imageAttributes
    );

typedef GpStatus (WINGDIPAPI *PFNGdipEnumerateMetafileDestPointI)(
    GpGraphics *            graphics,
    GDIPCONST GpMetafile *  metafile,
    GDIPCONST Point &       destPoint,
    EnumerateMetafileProc   callback,
    VOID *                  callbackData,
    GDIPCONST GpImageAttributes *     imageAttributes
    );

typedef GpStatus (WINGDIPAPI *PFNGdipEnumerateMetafileDestRect)(
    GpGraphics *            graphics,
    GDIPCONST GpMetafile *  metafile,
    GDIPCONST RectF &       destRect,
    EnumerateMetafileProc   callback,
    VOID *                  callbackData,
    GDIPCONST GpImageAttributes *     imageAttributes
    );

typedef GpStatus (WINGDIPAPI *PFNGdipEnumerateMetafileDestRectI)(
    GpGraphics *            graphics,
    GDIPCONST GpMetafile *  metafile,
    GDIPCONST Rect &        destRect,
    EnumerateMetafileProc   callback,
    VOID *                  callbackData,
    GDIPCONST GpImageAttributes *     imageAttributes
    );

typedef GpStatus (WINGDIPAPI *PFNGdipEnumerateMetafileDestPoints)(
    GpGraphics *            graphics,
    GDIPCONST GpMetafile *  metafile,
    GDIPCONST PointF *      destPoints,
    INT                     count,
    EnumerateMetafileProc   callback,
    VOID *                  callbackData,
    GDIPCONST GpImageAttributes *     imageAttributes
    );

typedef GpStatus (WINGDIPAPI *PFNGdipEnumerateMetafileDestPointsI)(
    GpGraphics *            graphics,
    GDIPCONST GpMetafile *  metafile,
    GDIPCONST Point *       destPoints,
    INT                     count,
    EnumerateMetafileProc   callback,
    VOID *                  callbackData,
    GDIPCONST GpImageAttributes *     imageAttributes
    );

typedef GpStatus (WINGDIPAPI *PFNGdipEnumerateMetafileSrcRectDestPoint)(
    GpGraphics *            graphics,
    GDIPCONST GpMetafile *  metafile,
    GDIPCONST PointF &      destPoint,
    GDIPCONST RectF &       srcRect,
    Unit                    srcUnit,
    EnumerateMetafileProc   callback,
    VOID *                  callbackData,
    GDIPCONST GpImageAttributes *     imageAttributes
    );

typedef GpStatus (WINGDIPAPI *PFNGdipEnumerateMetafileSrcRectDestPointI)(
    GpGraphics *            graphics,
    GDIPCONST GpMetafile *  metafile,
    GDIPCONST Point &       destPoint,
    GDIPCONST Rect &        srcRect,
    Unit                    srcUnit,
    EnumerateMetafileProc   callback,
    VOID *                  callbackData,
    GDIPCONST GpImageAttributes *     imageAttributes
    );

typedef GpStatus (WINGDIPAPI *PFNGdipEnumerateMetafileSrcRectDestRect)(
    GpGraphics *            graphics,
    GDIPCONST GpMetafile *  metafile,
    GDIPCONST RectF &       destRect,
    GDIPCONST RectF &       srcRect,
    Unit                    srcUnit,
    EnumerateMetafileProc   callback,
    VOID *                  callbackData,
    GDIPCONST GpImageAttributes *     imageAttributes
    );

typedef GpStatus (WINGDIPAPI *PFNGdipEnumerateMetafileSrcRectDestRectI)(
    GpGraphics *            graphics,
    GDIPCONST GpMetafile *  metafile,
    GDIPCONST Rect &        destRect,
    GDIPCONST Rect &        srcRect,
    Unit                    srcUnit,
    EnumerateMetafileProc   callback,
    VOID *                  callbackData,
    GDIPCONST GpImageAttributes *     imageAttributes
    );

typedef GpStatus (WINGDIPAPI *PFNGdipEnumerateMetafileSrcRectDestPoints)(
    GpGraphics *            graphics,
    GDIPCONST GpMetafile *  metafile,
    GDIPCONST PointF *      destPoints,
    INT                     count,
    GDIPCONST RectF &       srcRect,
    Unit                    srcUnit,
    EnumerateMetafileProc   callback,
    VOID *                  callbackData,
    GDIPCONST GpImageAttributes *     imageAttributes
    );

typedef GpStatus (WINGDIPAPI *PFNGdipEnumerateMetafileSrcRectDestPointsI)(
    GpGraphics *            graphics,
    GDIPCONST GpMetafile *  metafile,
    GDIPCONST Point *       destPoints,
    INT                     count,
    GDIPCONST Rect &        srcRect,
    Unit                    srcUnit,
    EnumerateMetafileProc   callback,
    VOID *                  callbackData,
    GDIPCONST GpImageAttributes *     imageAttributes
    );

typedef GpStatus (WINGDIPAPI *PFNGdipPlayMetafileRecord)(
    GDIPCONST GpMetafile *  metafile,
    EmfPlusRecordType       recordType,
    UINT                    flags,
    UINT                    dataSize,
    GDIPCONST BYTE *        data
    );

typedef GpStatus (WINGDIPAPI *PFNGdipSetClipGraphics)(GpGraphics *graphics, GpGraphics *srcgraphics,
                    CombineMode combineMode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetClipRect)(GpGraphics *graphics, REAL x, REAL y,
                         REAL width, REAL height, CombineMode combineMode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetClipRectI)(GpGraphics *graphics, INT x, INT y,
                         INT width, INT height, CombineMode combineMode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetClipPath)(GpGraphics *graphics, GpPath *path, CombineMode combineMode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetClipRegion)(GpGraphics *graphics, GpRegion *region,
                  CombineMode combineMode);

typedef GpStatus (WINGDIPAPI *PFNGdipSetClipHrgn)(GpGraphics *graphics, HRGN hRgn, CombineMode combineMode);

typedef GpStatus (WINGDIPAPI *PFNGdipResetClip)(GpGraphics *graphics);

typedef GpStatus (WINGDIPAPI *PFNGdipTranslateClip)(GpGraphics *graphics, REAL dx, REAL dy);

typedef GpStatus (WINGDIPAPI *PFNGdipTranslateClipI)(GpGraphics *graphics, INT dx, INT dy);

typedef GpStatus (WINGDIPAPI *PFNGdipGetClip)(GpGraphics *graphics, GpRegion *region);

typedef GpStatus (WINGDIPAPI *PFNGdipGetClipBounds)(GpGraphics *graphics, GpRectF *rect);

typedef GpStatus (WINGDIPAPI *PFNGdipGetClipBoundsI)(GpGraphics *graphics, GpRect *rect);

typedef GpStatus (WINGDIPAPI *PFNGdipIsClipEmpty)(GpGraphics *graphics, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipGetVisibleClipBounds)(GpGraphics *graphics, GpRectF *rect);

typedef GpStatus (WINGDIPAPI *PFNGdipGetVisibleClipBoundsI)(GpGraphics *graphics, GpRect *rect);

typedef GpStatus (WINGDIPAPI *PFNGdipIsVisibleClipEmpty)(GpGraphics *graphics, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsVisiblePoint)(GpGraphics *graphics, REAL x, REAL y,
                           BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsVisiblePointI)(GpGraphics *graphics, INT x, INT y,
                           BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsVisibleRect)(GpGraphics *graphics, REAL x, REAL y,
                           REAL width, REAL height, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipIsVisibleRectI)(GpGraphics *graphics, INT x, INT y,
                           INT width, INT height, BOOL *result);

typedef GpStatus (WINGDIPAPI *PFNGdipSaveGraphics)(GpGraphics *graphics, GraphicsState *state);

typedef GpStatus (WINGDIPAPI *PFNGdipRestoreGraphics)(GpGraphics *graphics, GraphicsState state);

typedef GpStatus (WINGDIPAPI *PFNGdipBeginContainer)(GpGraphics *graphics, GDIPCONST GpRectF* dstrect,
                   GDIPCONST GpRectF *srcrect, GpUnit unit, 
                   GraphicsContainer *state);

typedef GpStatus (WINGDIPAPI *PFNGdipBeginContainerI)(GpGraphics *graphics, GDIPCONST GpRect* dstrect,
                    GDIPCONST GpRect *srcrect, GpUnit unit, 
                    GraphicsContainer *state);

typedef GpStatus (WINGDIPAPI *PFNGdipBeginContainer2)(GpGraphics *graphics, GraphicsContainer* state);

typedef GpStatus (WINGDIPAPI *PFNGdipEndContainer)(GpGraphics *graphics, GraphicsContainer state);

typedef GpStatus (*PFNGdipGetMetafileHeaderFromWmf)(
    HMETAFILE           hWmf,
    GDIPCONST WmfPlaceableFileHeader *     wmfPlaceableFileHeader,
    MetafileHeader *    header
    );

typedef GpStatus (WINGDIPAPI *PFNGdipGetMetafileHeaderFromEmf)(
    HENHMETAFILE        hEmf,
    MetafileHeader *    header
    );

typedef GpStatus (WINGDIPAPI *PFNGdipGetMetafileHeaderFromFile)(
    GDIPCONST WCHAR*        filename,
    MetafileHeader *    header
    );

typedef GpStatus (WINGDIPAPI *PFNGdipGetMetafileHeaderFromStream)(
    IStream *           stream,
    MetafileHeader *    header
    );

typedef GpStatus (WINGDIPAPI *PFNGdipGetMetafileHeaderFromMetafile)(
    GpMetafile *        metafile,
    MetafileHeader *    header
    );

typedef GpStatus (WINGDIPAPI *PFNGdipGetHemfFromMetafile)(
    GpMetafile *        metafile,
    HENHMETAFILE *      hEmf
    );

typedef GpStatus (WINGDIPAPI *PFNGdipCreateStreamOnFile)(GDIPCONST WCHAR * filename, UINT access, 
                       IStream **stream);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateMetafileFromWmf)(HMETAFILE hWmf, BOOL deleteWmf,
                          GDIPCONST WmfPlaceableFileHeader * wmfPlaceableFileHeader, 
                          GpMetafile **metafile);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateMetafileFromEmf)(HENHMETAFILE hEmf, BOOL deleteEmf,
                          GpMetafile **metafile);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateMetafileFromFile)(GDIPCONST WCHAR* file, GpMetafile **metafile);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateMetafileFromWmfFile)(GDIPCONST WCHAR* file, 
                              GDIPCONST WmfPlaceableFileHeader * wmfPlaceableFileHeader, 
                              GpMetafile **metafile);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateMetafileFromStream)(IStream * stream, GpMetafile **metafile);

typedef GpStatus (WINGDIPAPI *PFNGdipRecordMetafile)(
    HDC                 referenceHdc,
    EmfType             type,
    GDIPCONST GpRectF * frameRect,
    MetafileFrameUnit   frameUnit,
    GDIPCONST WCHAR *   description,
    GpMetafile **       metafile
    );

typedef GpStatus (WINGDIPAPI *PFNGdipRecordMetafileI)(
    HDC                 referenceHdc,
    EmfType             type,
    GDIPCONST GpRect *  frameRect,
    MetafileFrameUnit   frameUnit,
    GDIPCONST WCHAR *   description,
    GpMetafile **       metafile
    );

typedef GpStatus (WINGDIPAPI *PFNGdipRecordMetafileFileName)(
    GDIPCONST WCHAR*    fileName,
    HDC                 referenceHdc,
    EmfType             type,
    GDIPCONST GpRectF * frameRect,
    MetafileFrameUnit   frameUnit,
    GDIPCONST WCHAR *   description,
    GpMetafile **       metafile
    );

typedef GpStatus (WINGDIPAPI *PFNGdipRecordMetafileFileNameI)(
    GDIPCONST WCHAR*    fileName,
    HDC                 referenceHdc,
    EmfType             type,
    GDIPCONST GpRect *  frameRect,
    MetafileFrameUnit   frameUnit,
    GDIPCONST WCHAR *   description,
    GpMetafile **       metafile
    );

typedef GpStatus (WINGDIPAPI *PFNGdipRecordMetafileStream)(
    IStream *           stream,
    HDC                 referenceHdc,
    EmfType             type,
    GDIPCONST GpRectF * frameRect,
    MetafileFrameUnit   frameUnit,
    GDIPCONST WCHAR *   description,
    GpMetafile **       metafile
    );

typedef GpStatus (WINGDIPAPI *PFNGdipRecordMetafileStreamI)(
    IStream *           stream,
    HDC                 referenceHdc,
    EmfType             type,
    GDIPCONST GpRect *  frameRect,
    MetafileFrameUnit   frameUnit,
    GDIPCONST WCHAR *   description,
    GpMetafile **       metafile
    );

typedef GpStatus (WINGDIPAPI *PFNGdipSetMetafileDownLevelRasterizationLimit)(
    GpMetafile *            metafile,
    UINT                    metafileRasterizationLimitDpi
    );

typedef GpStatus (WINGDIPAPI *PFNGdipGetMetafileDownLevelRasterizationLimit)(
    GDIPCONST GpMetafile *  metafile,
    UINT *                  metafileRasterizationLimitDpi
    );

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageDecodersSize)(UINT *numDecoders, UINT *size);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageDecoders)(UINT numDecoders,
                     UINT size,
                     ImageCodecInfo *decoders);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageEncodersSize)(UINT *numEncoders, UINT *size);

typedef GpStatus (WINGDIPAPI *PFNGdipGetImageEncoders)(UINT numEncoders,
                     UINT size,
                     ImageCodecInfo *encoders);

typedef GpStatus (WINGDIPAPI *PFNGdipComment)(GpGraphics* graphics, UINT sizeData, GDIPCONST BYTE * data);

//----------------------------------------------------------------------------
// FontFamily APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateFontFamilyFromName)(GDIPCONST WCHAR *name,
                             GpFontCollection *fontCollection,
                             GpFontFamily **FontFamily);

typedef GpStatus (WINGDIPAPI *PFNGdipDeleteFontFamily)(GpFontFamily *FontFamily);

typedef GpStatus (WINGDIPAPI *PFNGdipCloneFontFamily)(GpFontFamily *FontFamily, GpFontFamily **clonedFontFamily);

typedef GpStatus (WINGDIPAPI *PFNGdipGetGenericFontFamilySansSerif)(GpFontFamily **nativeFamily);

typedef GpStatus (WINGDIPAPI *PFNGdipGetGenericFontFamilySerif)(GpFontFamily **nativeFamily);

typedef GpStatus (WINGDIPAPI *PFNGdipGetGenericFontFamilyMonospace)(GpFontFamily **nativeFamily);


typedef GpStatus (WINGDIPAPI *PFNGdipGetFamilyName)(
    GDIPCONST GpFontFamily  *family,
    WCHAR                name[LF_FACESIZE],
    LANGID               language
);

typedef GpStatus (WINGDIPAPI *PFNGdipIsStyleAvailable)(GDIPCONST GpFontFamily *family, INT style, 
                     BOOL * IsStyleAvailable);

typedef GpStatus (WINGDIPAPI *PFNGdipFontCollectionEnumerable)(
    GpFontCollection* fontCollection,
    GpGraphics* graphics,
    INT *       numFound
);

typedef GpStatus (WINGDIPAPI *PFNGdipFontCollectionEnumerate)(
    GpFontCollection* fontCollection,
    INT             numSought,
    GpFontFamily*   gpfamilies[],
    INT*            numFound,
    GpGraphics*     graphics
);

typedef GpStatus (WINGDIPAPI *PFNGdipGetEmHeight)(GDIPCONST GpFontFamily *family, INT style, 
                UINT16 * EmHeight);

typedef GpStatus (WINGDIPAPI *PFNGdipGetCellAscent)(GDIPCONST GpFontFamily *family, INT style,
                  UINT16 * CellAscent);

typedef GpStatus (WINGDIPAPI *PFNGdipGetCellDescent)(GDIPCONST GpFontFamily *family, INT style, 
                   UINT16 * CellDescent);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLineSpacing)(GDIPCONST GpFontFamily *family, INT style, 
                   UINT16 * LineSpacing);


//----------------------------------------------------------------------------
// Font APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateFontFromDC)(
    HDC        hdc,
    GpFont   **font
);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateFontFromLogfontA)(
    HDC        hdc,
    GDIPCONST LOGFONTA  *logfont,
    GpFont   **font
);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateFontFromLogfontW)(
    HDC        hdc,
    GDIPCONST LOGFONTW  *logfont,
    GpFont   **font
);

typedef GpStatus (WINGDIPAPI *PFNGdipCreateFont)(
    GDIPCONST GpFontFamily  *fontFamily,
    REAL                 emSize,
    INT                  style,
    Unit                 unit,
    GpFont             **font
);

typedef GpStatus (WINGDIPAPI *PFNGdipCloneFont)(GpFont* font, GpFont** cloneFont);

typedef GpStatus (WINGDIPAPI *PFNGdipDeleteFont)(GpFont* font);

typedef GpStatus (WINGDIPAPI *PFNGdipGetFamily)(GpFont *font, GpFontFamily **family);

typedef GpStatus (WINGDIPAPI *PFNGdipGetFontStyle)(GpFont *font, INT *style);

typedef GpStatus (WINGDIPAPI *PFNGdipGetFontSize)(GpFont *font, REAL *size);

typedef GpStatus (WINGDIPAPI *PFNGdipGetFontUnit)(GpFont *font, Unit *unit);

typedef GpStatus (WINGDIPAPI *PFNGdipGetFontHeight)(GDIPCONST GpFont *font, GDIPCONST GpGraphics *graphics, 
                  REAL *height);

typedef GpStatus (WINGDIPAPI *PFNGdipGetFontHeightGivenDPI)(GDIPCONST GpFont *font, REAL dpi, REAL *height);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLogFontA)(GpFont * font, GpGraphics *graphics, LOGFONTA * logfontA);

typedef GpStatus (WINGDIPAPI *PFNGdipGetLogFontW)(GpFont * font, GpGraphics *graphics, LOGFONTW * logfontW);

typedef GpStatus (WINGDIPAPI *PFNGdipNewInstalledFontCollection)(GpFontCollection** fontCollection);

typedef GpStatus (WINGDIPAPI *PFNGdipNewPrivateFontCollection)(GpFontCollection** fontCollection);

typedef GpStatus (WINGDIPAPI *PFNGdipDeletePrivateFontCollection)(GpFontCollection** fontCollection);

typedef GpStatus (WINGDIPAPI *PFNGdipGetFontCollectionFamilyCount)(
    GpFontCollection* fontCollection,
    INT *       numFound
);

typedef GpStatus (WINGDIPAPI *PFNGdipGetFontCollectionFamilyList)(
    GpFontCollection* fontCollection,
    INT             numSought,
    GpFontFamily*   gpfamilies[],
    INT*            numFound
);

typedef GpStatus (WINGDIPAPI *PFNGdipPrivateAddFontFile)(
    GpFontCollection* fontCollection,
    GDIPCONST WCHAR* filename
);

typedef GpStatus (WINGDIPAPI *PFNGdipPrivateAddMemoryFont)(
    GpFontCollection* fontCollection,
    GDIPCONST void* memory,
    INT length
);

//----------------------------------------------------------------------------
// Text APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipDrawString)(
    GpGraphics               *graphics,
    GDIPCONST WCHAR          *string,
    INT                       length,
    GDIPCONST GpFont         *font,
    GDIPCONST RectF          *layoutRect,
    GDIPCONST GpStringFormat *stringFormat,
    GDIPCONST GpBrush        *brush
);

typedef GpStatus (WINGDIPAPI *PFNGdipMeasureString)(
    GpGraphics               *graphics,
    GDIPCONST WCHAR          *string,
    INT                       length,
    GDIPCONST GpFont         *font,
    GDIPCONST RectF          *layoutRect,
    GDIPCONST GpStringFormat *stringFormat,
    RectF                    *boundingBox,
    INT                      *codepointsFitted,
    INT                      *linesFilled
);

typedef GpStatus (WINGDIPAPI *PFNGdipMeasureCharacterRanges)(
    GpGraphics               *graphics,
    GDIPCONST WCHAR          *string,
    INT                       length,
    GDIPCONST GpFont         *font,
    GDIPCONST RectF          &layoutRect,
    GDIPCONST GpStringFormat *stringFormat,
    INT                       regionCount,
    GpRegion                **regions
);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawDriverString)(
    GpGraphics *graphics,
    GDIPCONST UINT16 *text,
    INT length,
    GDIPCONST GpFont *font,
    GDIPCONST GpBrush *brush,
    GDIPCONST PointF *positions,
    INT flags,
    GDIPCONST GpMatrix *matrix
);

typedef GpStatus (WINGDIPAPI *PFNGdipMeasureDriverString)(
    GpGraphics *graphics,
    GDIPCONST UINT16 *text,
    INT length,
    GDIPCONST GpFont *font,
    GDIPCONST PointF *positions,
    INT flags,
    GDIPCONST GpMatrix *matrix,
    RectF *boundingBox
);

//----------------------------------------------------------------------------
// String format APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateStringFormat)(
    INT               formatAttributes,
    LANGID            language,
    GpStringFormat  **format
);

typedef GpStatus (WINGDIPAPI *PFNGdipStringFormatGetGenericDefault)(GpStringFormat **format);

typedef GpStatus (WINGDIPAPI *PFNGdipStringFormatGetGenericTypographic)(GpStringFormat **format);

typedef GpStatus (WINGDIPAPI *PFNGdipDeleteStringFormat)(GpStringFormat *format);

typedef GpStatus (WINGDIPAPI *PFNGdipCloneStringFormat)(GDIPCONST GpStringFormat *format, 
                      GpStringFormat **newFormat);

typedef GpStatus (WINGDIPAPI *PFNGdipSetStringFormatFlags)(GpStringFormat *format, INT flags);

typedef GpStatus (WINGDIPAPI *PFNGdipGetStringFormatFlags)(GDIPCONST GpStringFormat *format, 
                                             INT *flags);

typedef GpStatus (WINGDIPAPI *PFNGdipSetStringFormatAlign)(GpStringFormat *format, StringAlignment align);

typedef GpStatus (WINGDIPAPI *PFNGdipGetStringFormatAlign)(GDIPCONST GpStringFormat *format, 
                         StringAlignment *align);

typedef GpStatus (WINGDIPAPI *PFNGdipSetStringFormatLineAlign)(GpStringFormat *format,
                             StringAlignment align);

typedef GpStatus (WINGDIPAPI *PFNGdipGetStringFormatLineAlign)(GDIPCONST GpStringFormat *format,
                             StringAlignment *align);

typedef GpStatus (WINGDIPAPI *PFNGdipSetStringFormatTrimming)(
    GpStringFormat  *format,
    StringTrimming   trimming
);

typedef GpStatus (WINGDIPAPI *PFNGdipGetStringFormatTrimming)(
    GDIPCONST GpStringFormat *format,
    StringTrimming       *trimming
);

typedef GpStatus (WINGDIPAPI *PFNGdipSetStringFormatHotkeyPrefix)(GpStringFormat *format, INT hotkeyPrefix);

typedef GpStatus (WINGDIPAPI *PFNGdipGetStringFormatHotkeyPrefix)(GDIPCONST GpStringFormat *format, 
                                INT *hotkeyPrefix);

typedef GpStatus (WINGDIPAPI *PFNGdipSetStringFormatTabStops)(GpStringFormat *format, REAL firstTabOffset, 
                            INT count, GDIPCONST REAL *tabStops);

typedef GpStatus (WINGDIPAPI *PFNGdipGetStringFormatTabStops)(GDIPCONST GpStringFormat *format, INT count, 
                            REAL *firstTabOffset, REAL *tabStops);

typedef GpStatus (WINGDIPAPI *PFNGdipGetStringFormatTabStopCount)(GDIPCONST GpStringFormat *format, INT * count);

typedef GpStatus (WINGDIPAPI *PFNGdipSetStringFormatDigitSubstitution)(GpStringFormat *format, LANGID language,
                                     StringDigitSubstitute substitute);

typedef GpStatus (WINGDIPAPI *PFNGdipGetStringFormatDigitSubstitution)(GDIPCONST GpStringFormat *format, 
                                     LANGID *language,
                                     StringDigitSubstitute *substitute);

typedef GpStatus (WINGDIPAPI *PFNGdipGetStringFormatMeasurableCharacterRangeCount)(
    GDIPCONST GpStringFormat    *format,
    INT                         *count
);

typedef GpStatus (WINGDIPAPI *PFNGdipSetStringFormatMeasurableCharacterRanges)(
    GpStringFormat              *format,
    INT                         rangeCount,
    GDIPCONST CharacterRange    *ranges
);

//----------------------------------------------------------------------------
// Cached Bitmap APIs
//----------------------------------------------------------------------------

typedef GpStatus (WINGDIPAPI *PFNGdipCreateCachedBitmap)(
    GpBitmap *bitmap,
    GpGraphics *graphics,
    GpCachedBitmap **cachedBitmap
);

typedef GpStatus (WINGDIPAPI *PFNGdipDeleteCachedBitmap)(GpCachedBitmap *cachedBitmap);

typedef GpStatus (WINGDIPAPI *PFNGdipDrawCachedBitmap)(
    GpGraphics *graphics,
    GpCachedBitmap *cachedBitmap,
    INT x,
    INT y
);

typedef UINT(WINGDIPAPI *PFNGdipEmfToWmfBits)(
    HENHMETAFILE hemf,
    UINT         cbData16,
    LPBYTE       pData16,
    INT          iMapMode,
    INT          eFlags
);

typedef GpStatus (WINGDIPAPI *PFNGdipSetImageAttributesCachedBackground)(
    GpImageAttributes *imageattr,
    BOOL enableFlag
);

typedef GpStatus (WINGDIPAPI *PFNGdipTestControl)(
    GpTestControlEnum control,
    void * param
);

typedef GpStatus (WINAPI *PFNGdiplusNotificationHook)(
    OUT ULONG_PTR *token
);

typedef VOID(WINAPI *PFNGdiplusNotificationUnhook)(
    ULONG_PTR token
);

#ifdef GDIP_NO_EXTERN
	#define Declare(name) PFN##name name
#else
	#define Declare(name) extern PFN##name name
#endif GDIP_NO_EXTERN 

// Declaration List
Declare(GdipCreatePath);
Declare(GdipCreatePath2);
Declare(GdipCreatePath2I);
Declare(GdipClonePath);
Declare(GdipDeletePath);
Declare(GdipResetPath);
Declare(GdipGetPointCount);
Declare(GdipGetPathTypes);
Declare(GdipGetPathPoints);
Declare(GdipGetPathPointsI);
Declare(GdipGetPathFillMode);
Declare(GdipSetPathFillMode);
Declare(GdipGetPathData);
Declare(GdipStartPathFigure);
Declare(GdipClosePathFigure);
Declare(GdipClosePathFigures);
Declare(GdipSetPathMarker);
Declare(GdipClearPathMarkers);
Declare(GdipReversePath);
Declare(GdipGetPathLastPoint);
Declare(GdipAddPathLine);
Declare(GdipAddPathLine2);
Declare(GdipAddPathArc);
Declare(GdipAddPathBezier);
Declare(GdipAddPathBeziers);
Declare(GdipAddPathCurve);
Declare(GdipAddPathCurve2);
Declare(GdipAddPathCurve3);
Declare(GdipAddPathClosedCurve);
Declare(GdipAddPathClosedCurve2);
Declare(GdipAddPathRectangle);
Declare(GdipAddPathRectangles);
Declare(GdipAddPathEllipse);
Declare(GdipAddPathPie);
Declare(GdipAddPathPolygon);
Declare(GdipAddPathPath);
Declare(GdipAddPathString);
Declare(GdipAddPathStringI);
Declare(GdipAddPathLineI);
Declare(GdipAddPathLine2I);
Declare(GdipAddPathArcI);
Declare(GdipAddPathBezierI);
Declare(GdipAddPathBeziersI);
Declare(GdipAddPathCurveI);
Declare(GdipAddPathCurve2I);
Declare(GdipAddPathCurve3I);
Declare(GdipAddPathClosedCurveI);
Declare(GdipAddPathClosedCurve2I);
Declare(GdipAddPathRectangleI);
Declare(GdipAddPathRectanglesI);
Declare(GdipAddPathEllipseI);
Declare(GdipAddPathPieI);
Declare(GdipAddPathPolygonI);
Declare(GdipFlattenPath);
Declare(GdipWindingModeOutline);
Declare(GdipWidenPath);
Declare(GdipWarpPath);
Declare(GdipTransformPath);
Declare(GdipGetPathWorldBounds);
Declare(GdipGetPathWorldBoundsI);
Declare(GdipIsVisiblePathPoint);
Declare(GdipIsVisiblePathPointI);
Declare(GdipIsOutlineVisiblePathPoint);
Declare(GdipIsOutlineVisiblePathPointI);
Declare(GdipCreatePathIter);
Declare(GdipDeletePathIter);
Declare(GdipPathIterNextSubpath);
Declare(GdipPathIterNextSubpathPath);
Declare(GdipPathIterNextPathType);
Declare(GdipPathIterNextMarker);
Declare(GdipPathIterNextMarkerPath);
Declare(GdipPathIterGetCount);
Declare(GdipPathIterGetSubpathCount);
Declare(GdipPathIterIsValid);
Declare(GdipPathIterHasCurve);
Declare(GdipPathIterRewind);
Declare(GdipPathIterEnumerate);
Declare(GdipPathIterCopyData);
Declare(GdipCreateMatrix);
Declare(GdipCreateMatrix2);
Declare(GdipCreateMatrix3);
Declare(GdipCreateMatrix3I);
Declare(GdipCloneMatrix);
Declare(GdipDeleteMatrix);
Declare(GdipSetMatrixElements);
Declare(GdipMultiplyMatrix);
Declare(GdipTranslateMatrix);
Declare(GdipScaleMatrix);
Declare(GdipRotateMatrix);
Declare(GdipShearMatrix);
Declare(GdipInvertMatrix);
Declare(GdipTransformMatrixPoints);
Declare(GdipTransformMatrixPointsI);
Declare(GdipVectorTransformMatrixPoints);
Declare(GdipVectorTransformMatrixPointsI);
Declare(GdipGetMatrixElements);
Declare(GdipIsMatrixInvertible);
Declare(GdipIsMatrixIdentity);
Declare(GdipIsMatrixEqual);
Declare(GdipCreateRegion);
Declare(GdipCreateRegionRect);
Declare(GdipCreateRegionRectI);
Declare(GdipCreateRegionPath);
Declare(GdipCreateRegionRgnData);
Declare(GdipCreateRegionHrgn);
Declare(GdipCloneRegion);
Declare(GdipDeleteRegion);
Declare(GdipSetInfinite);
Declare(GdipSetEmpty);
Declare(GdipCombineRegionRect);
Declare(GdipCombineRegionRectI);
Declare(GdipCombineRegionPath);
Declare(GdipCombineRegionRegion);
Declare(GdipTranslateRegion);
Declare(GdipTranslateRegionI);
Declare(GdipTransformRegion);
Declare(GdipGetRegionBounds);
Declare(GdipGetRegionBoundsI);
Declare(GdipGetRegionHRgn);
Declare(GdipIsEmptyRegion);
Declare(GdipIsInfiniteRegion);
Declare(GdipIsEqualRegion);
Declare(GdipGetRegionDataSize);
Declare(GdipGetRegionData);
Declare(GdipIsVisibleRegionPoint);
Declare(GdipIsVisibleRegionPointI);
Declare(GdipIsVisibleRegionRect);
Declare(GdipIsVisibleRegionRectI);
Declare(GdipGetRegionScansCount);
Declare(GdipGetRegionScans);
Declare(GdipGetRegionScansI);
Declare(GdipCloneBrush);
Declare(GdipDeleteBrush);
Declare(GdipGetBrushType);
Declare(GdipCreateHatchBrush);
Declare(GdipGetHatchStyle);
Declare(GdipGetHatchForegroundColor);
Declare(GdipGetHatchBackgroundColor);
Declare(GdipCreateTexture);
Declare(GdipCreateTexture2);
Declare(GdipCreateTextureIA);
Declare(GdipCreateTexture2I);
Declare(GdipCreateTextureIAI);
Declare(GdipGetTextureTransform);
Declare(GdipSetTextureTransform);
Declare(GdipResetTextureTransform);
Declare(GdipMultiplyTextureTransform);
Declare(GdipTranslateTextureTransform);
Declare(GdipScaleTextureTransform);
Declare(GdipRotateTextureTransform);
Declare(GdipSetTextureWrapMode);
Declare(GdipGetTextureWrapMode);
Declare(GdipGetTextureImage);
Declare(GdipCreateSolidFill);
Declare(GdipSetSolidFillColor);
Declare(GdipGetSolidFillColor);
Declare(GdipCreateLineBrush);
Declare(GdipCreateLineBrushI);
Declare(GdipCreateLineBrushFromRect);
Declare(GdipCreateLineBrushFromRectI);
Declare(GdipCreateLineBrushFromRectWithAngle);
Declare(GdipCreateLineBrushFromRectWithAngleI);
Declare(GdipSetLineColors);
Declare(GdipGetLineColors);
Declare(GdipGetLineRect);
Declare(GdipGetLineRectI);
Declare(GdipSetLineGammaCorrection);
Declare(GdipGetLineGammaCorrection);
Declare(GdipGetLineBlendCount);
Declare(GdipGetLineBlend);
Declare(GdipSetLineBlend);
Declare(GdipGetLinePresetBlendCount);
Declare(GdipGetLinePresetBlend);
Declare(GdipSetLinePresetBlend);
Declare(GdipSetLineSigmaBlend);
Declare(GdipSetLineLinearBlend);
Declare(GdipSetLineWrapMode);
Declare(GdipGetLineWrapMode);
Declare(GdipGetLineTransform);
Declare(GdipSetLineTransform);
Declare(GdipResetLineTransform);
Declare(GdipMultiplyLineTransform);
Declare(GdipTranslateLineTransform);
Declare(GdipScaleLineTransform);
Declare(GdipRotateLineTransform);
Declare(GdipCreatePathGradient);
Declare(GdipCreatePathGradientI);
Declare(GdipCreatePathGradientFromPath);
Declare(GdipGetPathGradientCenterColor);
Declare(GdipSetPathGradientCenterColor);
Declare(GdipGetPathGradientSurroundColorsWithCount);
Declare(GdipSetPathGradientSurroundColorsWithCount);
Declare(GdipGetPathGradientPath);
Declare(GdipSetPathGradientPath);
Declare(GdipGetPathGradientCenterPoint);
Declare(GdipGetPathGradientCenterPointI);
Declare(GdipSetPathGradientCenterPoint);
Declare(GdipSetPathGradientCenterPointI);
Declare(GdipGetPathGradientRect);
Declare(GdipGetPathGradientRectI);
Declare(GdipGetPathGradientPointCount);
Declare(GdipGetPathGradientSurroundColorCount);
Declare(GdipSetPathGradientGammaCorrection);
Declare(GdipGetPathGradientGammaCorrection);
Declare(GdipGetPathGradientBlendCount);
Declare(GdipGetPathGradientBlend);
Declare(GdipSetPathGradientBlend);
Declare(GdipGetPathGradientPresetBlendCount);
Declare(GdipGetPathGradientPresetBlend);
Declare(GdipSetPathGradientPresetBlend);
Declare(GdipSetPathGradientSigmaBlend);
Declare(GdipSetPathGradientLinearBlend);
Declare(GdipGetPathGradientWrapMode);
Declare(GdipSetPathGradientWrapMode);
Declare(GdipGetPathGradientTransform);
Declare(GdipSetPathGradientTransform);
Declare(GdipResetPathGradientTransform);
Declare(GdipMultiplyPathGradientTransform);
Declare(GdipTranslatePathGradientTransform);
Declare(GdipScalePathGradientTransform);
Declare(GdipRotatePathGradientTransform);
Declare(GdipGetPathGradientFocusScales);
Declare(GdipSetPathGradientFocusScales);
Declare(GdipCreatePen1);
Declare(GdipCreatePen2);
Declare(GdipClonePen);
Declare(GdipDeletePen);
Declare(GdipSetPenWidth);
Declare(GdipGetPenWidth);
Declare(GdipSetPenUnit);
Declare(GdipGetPenUnit);
Declare(GdipSetPenLineCap197819);
Declare(GdipSetPenStartCap);
Declare(GdipSetPenEndCap);
Declare(GdipSetPenDashCap197819);
Declare(GdipGetPenStartCap);
Declare(GdipGetPenEndCap);
Declare(GdipGetPenDashCap197819);
Declare(GdipSetPenLineJoin);
Declare(GdipGetPenLineJoin);
Declare(GdipSetPenCustomStartCap);
Declare(GdipGetPenCustomStartCap);
Declare(GdipSetPenCustomEndCap);
Declare(GdipGetPenCustomEndCap);
Declare(GdipSetPenMiterLimit);
Declare(GdipGetPenMiterLimit);
Declare(GdipSetPenMode);
Declare(GdipGetPenMode);
Declare(GdipSetPenTransform);
Declare(GdipGetPenTransform);
Declare(GdipResetPenTransform);
Declare(GdipMultiplyPenTransform);
Declare(GdipTranslatePenTransform);
Declare(GdipScalePenTransform);
Declare(GdipRotatePenTransform);
Declare(GdipSetPenColor);
Declare(GdipGetPenColor);
Declare(GdipSetPenBrushFill);
Declare(GdipGetPenBrushFill);
Declare(GdipGetPenFillType);
Declare(GdipGetPenDashStyle);
Declare(GdipSetPenDashStyle);
Declare(GdipGetPenDashOffset);
Declare(GdipSetPenDashOffset);
Declare(GdipGetPenDashCount);
Declare(GdipSetPenDashArray);
Declare(GdipGetPenDashArray);
Declare(GdipGetPenCompoundCount);
Declare(GdipSetPenCompoundArray);
Declare(GdipGetPenCompoundArray);
Declare(GdipCreateCustomLineCap);
Declare(GdipDeleteCustomLineCap);
Declare(GdipCloneCustomLineCap);
Declare(GdipGetCustomLineCapType);
Declare(GdipSetCustomLineCapStrokeCaps);
Declare(GdipGetCustomLineCapStrokeCaps);
Declare(GdipSetCustomLineCapStrokeJoin);
Declare(GdipGetCustomLineCapStrokeJoin);
Declare(GdipSetCustomLineCapBaseCap);
Declare(GdipGetCustomLineCapBaseCap);
Declare(GdipSetCustomLineCapBaseInset);
Declare(GdipGetCustomLineCapBaseInset);
Declare(GdipSetCustomLineCapWidthScale);
Declare(GdipGetCustomLineCapWidthScale);
Declare(GdipCreateAdjustableArrowCap);
Declare(GdipSetAdjustableArrowCapHeight);
Declare(GdipGetAdjustableArrowCapHeight);
Declare(GdipSetAdjustableArrowCapWidth);
Declare(GdipGetAdjustableArrowCapWidth);
Declare(GdipSetAdjustableArrowCapMiddleInset);
Declare(GdipGetAdjustableArrowCapMiddleInset);
Declare(GdipSetAdjustableArrowCapFillState);
Declare(GdipGetAdjustableArrowCapFillState);
Declare(GdipLoadImageFromStream);
Declare(GdipLoadImageFromFile);
Declare(GdipLoadImageFromStreamICM);
Declare(GdipLoadImageFromFileICM);
Declare(GdipCloneImage);
Declare(GdipDisposeImage);
Declare(GdipSaveImageToFile);
Declare(GdipSaveImageToStream);
Declare(GdipSaveAdd);
Declare(GdipSaveAddImage);
Declare(GdipGetImageGraphicsContext);
Declare(GdipGetImageBounds);
Declare(GdipGetImageDimension);
Declare(GdipGetImageType);
Declare(GdipGetImageWidth);
Declare(GdipGetImageHeight);
Declare(GdipGetImageHorizontalResolution);
Declare(GdipGetImageVerticalResolution);
Declare(GdipGetImageFlags);
Declare(GdipGetImageRawFormat);
Declare(GdipGetImagePixelFormat);
Declare(GdipGetImageThumbnail);
Declare(GdipGetEncoderParameterListSize);
Declare(GdipGetEncoderParameterList);
Declare(GdipImageGetFrameDimensionsCount);
Declare(GdipImageGetFrameDimensionsList);
Declare(GdipImageGetFrameCount);
Declare(GdipImageSelectActiveFrame);
Declare(GdipImageRotateFlip);
Declare(GdipGetImagePalette);
Declare(GdipSetImagePalette);
Declare(GdipGetImagePaletteSize);
Declare(GdipGetPropertyCount);
Declare(GdipGetPropertyIdList);
Declare(GdipGetPropertyItemSize);
Declare(GdipGetPropertyItem);
Declare(GdipGetPropertySize);
Declare(GdipGetAllPropertyItems);
Declare(GdipRemovePropertyItem);
Declare(GdipSetPropertyItem);
Declare(GdipImageForceValidation);
Declare(GdipCreateBitmapFromStream);
Declare(GdipCreateBitmapFromFile);
Declare(GdipCreateBitmapFromStreamICM);
Declare(GdipCreateBitmapFromFileICM);
Declare(GdipCreateBitmapFromScan0);
Declare(GdipCreateBitmapFromGraphics);
Declare(GdipCreateBitmapFromDirectDrawSurface);
Declare(GdipCreateBitmapFromGdiDib);
Declare(GdipCreateBitmapFromHBITMAP);
Declare(GdipCreateHBITMAPFromBitmap);
Declare(GdipCreateBitmapFromHICON);
Declare(GdipCreateHICONFromBitmap);
Declare(GdipCreateBitmapFromResource);
Declare(GdipCloneBitmapArea);
Declare(GdipCloneBitmapAreaI);
Declare(GdipBitmapLockBits);
Declare(GdipBitmapUnlockBits);
Declare(GdipBitmapGetPixel);
Declare(GdipBitmapSetPixel);
Declare(GdipBitmapSetResolution);
Declare(GdipCreateImageAttributes);
Declare(GdipCloneImageAttributes);
Declare(GdipDisposeImageAttributes);
Declare(GdipSetImageAttributesToIdentity);
Declare(GdipResetImageAttributes);
Declare(GdipSetImageAttributesColorMatrix);
Declare(GdipSetImageAttributesThreshold);
Declare(GdipSetImageAttributesGamma);
Declare(GdipSetImageAttributesNoOp);
Declare(GdipSetImageAttributesColorKeys);
Declare(GdipSetImageAttributesOutputChannel);
Declare(GdipSetImageAttributesOutputChannelColorProfile);
Declare(GdipSetImageAttributesRemapTable);
Declare(GdipSetImageAttributesWrapMode);
Declare(GdipSetImageAttributesICMMode);
Declare(GdipGetImageAttributesAdjustedPalette);
Declare(GdipFlush);
Declare(GdipCreateFromHDC);
Declare(GdipCreateFromHDC2);
Declare(GdipCreateFromHWND);
Declare(GdipCreateFromHWNDICM);
Declare(GdipDeleteGraphics);
Declare(GdipGetDC);
Declare(GdipReleaseDC);
Declare(GdipSetCompositingMode);
Declare(GdipGetCompositingMode);
Declare(GdipSetRenderingOrigin);
Declare(GdipGetRenderingOrigin);
Declare(GdipSetCompositingQuality);
Declare(GdipGetCompositingQuality);
Declare(GdipSetSmoothingMode);
Declare(GdipGetSmoothingMode);
Declare(GdipSetPixelOffsetMode);
Declare(GdipGetPixelOffsetMode);
Declare(GdipSetTextRenderingHint);
Declare(GdipGetTextRenderingHint);
Declare(GdipSetTextContrast);
Declare(GdipGetTextContrast);
Declare(GdipSetInterpolationMode);
Declare(GdipGetInterpolationMode);
Declare(GdipSetWorldTransform);
Declare(GdipResetWorldTransform);
Declare(GdipMultiplyWorldTransform);
Declare(GdipTranslateWorldTransform);
Declare(GdipScaleWorldTransform);
Declare(GdipRotateWorldTransform);
Declare(GdipGetWorldTransform);
Declare(GdipResetPageTransform);
Declare(GdipGetPageUnit);
Declare(GdipGetPageScale);
Declare(GdipSetPageUnit);
Declare(GdipSetPageScale);
Declare(GdipGetDpiX);
Declare(GdipGetDpiY);
Declare(GdipTransformPoints);
Declare(GdipTransformPointsI);
Declare(GdipGetNearestColor);
Declare(GdipCreateHalftonePalette);
Declare(GdipDrawLine);
Declare(GdipDrawLineI);
Declare(GdipDrawLines);
Declare(GdipDrawLinesI);
Declare(GdipDrawArc);
Declare(GdipDrawArcI);
Declare(GdipDrawBezier);
Declare(GdipDrawBezierI);
Declare(GdipDrawBeziers);
Declare(GdipDrawBeziersI);
Declare(GdipDrawRectangle);
Declare(GdipDrawRectangleI);
Declare(GdipDrawRectangles);
Declare(GdipDrawRectanglesI);
Declare(GdipDrawEllipse);
Declare(GdipDrawEllipseI);
Declare(GdipDrawPie);
Declare(GdipDrawPieI);
Declare(GdipDrawPolygon);
Declare(GdipDrawPolygonI);
Declare(GdipDrawPath);
Declare(GdipDrawCurve);
Declare(GdipDrawCurveI);
Declare(GdipDrawCurve2);
Declare(GdipDrawCurve2I);
Declare(GdipDrawCurve3);
Declare(GdipDrawCurve3I);
Declare(GdipDrawClosedCurve);
Declare(GdipDrawClosedCurveI);
Declare(GdipDrawClosedCurve2);
Declare(GdipDrawClosedCurve2I);
Declare(GdipGraphicsClear);
Declare(GdipFillRectangle);
Declare(GdipFillRectangleI);
Declare(GdipFillRectangles);
Declare(GdipFillRectanglesI);
Declare(GdipFillPolygon);
Declare(GdipFillPolygonI);
Declare(GdipFillPolygon2);
Declare(GdipFillPolygon2I);
Declare(GdipFillEllipse);
Declare(GdipFillEllipseI);
Declare(GdipFillPie);
Declare(GdipFillPieI);
Declare(GdipFillPath);
Declare(GdipFillClosedCurve);
Declare(GdipFillClosedCurveI);
Declare(GdipFillClosedCurve2);
Declare(GdipFillClosedCurve2I);
Declare(GdipFillRegion);
Declare(GdipDrawImage);
Declare(GdipDrawImageI);
Declare(GdipDrawImageRect);
Declare(GdipDrawImageRectI);
Declare(GdipDrawImagePoints);
Declare(GdipDrawImagePointsI);
Declare(GdipDrawImagePointRect);
Declare(GdipDrawImagePointRectI);
Declare(GdipDrawImageRectRect);
Declare(GdipDrawImageRectRectI);
Declare(GdipDrawImagePointsRect);
Declare(GdipDrawImagePointsRectI);
Declare(GdipEnumerateMetafileDestPoint);
Declare(GdipEnumerateMetafileDestPointI);
Declare(GdipEnumerateMetafileDestRect);
Declare(GdipEnumerateMetafileDestRectI);
Declare(GdipEnumerateMetafileDestPoints);
Declare(GdipEnumerateMetafileDestPointsI);
Declare(GdipEnumerateMetafileSrcRectDestPoint);
Declare(GdipEnumerateMetafileSrcRectDestPointI);
Declare(GdipEnumerateMetafileSrcRectDestRect);
Declare(GdipEnumerateMetafileSrcRectDestRectI);
Declare(GdipEnumerateMetafileSrcRectDestPoints);
Declare(GdipEnumerateMetafileSrcRectDestPointsI);
Declare(GdipPlayMetafileRecord);
Declare(GdipSetClipGraphics);
Declare(GdipSetClipRect);
Declare(GdipSetClipRectI);
Declare(GdipSetClipPath);
Declare(GdipSetClipRegion);
Declare(GdipSetClipHrgn);
Declare(GdipResetClip);
Declare(GdipTranslateClip);
Declare(GdipTranslateClipI);
Declare(GdipGetClip);
Declare(GdipGetClipBounds);
Declare(GdipGetClipBoundsI);
Declare(GdipIsClipEmpty);
Declare(GdipGetVisibleClipBounds);
Declare(GdipGetVisibleClipBoundsI);
Declare(GdipIsVisibleClipEmpty);
Declare(GdipIsVisiblePoint);
Declare(GdipIsVisiblePointI);
Declare(GdipIsVisibleRect);
Declare(GdipIsVisibleRectI);
Declare(GdipSaveGraphics);
Declare(GdipRestoreGraphics);
Declare(GdipBeginContainer);
Declare(GdipBeginContainerI);
Declare(GdipBeginContainer2);
Declare(GdipEndContainer);
Declare(GdipGetMetafileHeaderFromWmf);
Declare(GdipGetMetafileHeaderFromEmf);
Declare(GdipGetMetafileHeaderFromFile);
Declare(GdipGetMetafileHeaderFromStream);
Declare(GdipGetMetafileHeaderFromMetafile);
Declare(GdipGetHemfFromMetafile);
Declare(GdipCreateStreamOnFile);
Declare(GdipCreateMetafileFromWmf);
Declare(GdipCreateMetafileFromEmf);
Declare(GdipCreateMetafileFromFile);
Declare(GdipCreateMetafileFromWmfFile);
Declare(GdipCreateMetafileFromStream);
Declare(GdipRecordMetafile);
Declare(GdipRecordMetafileI);
Declare(GdipRecordMetafileFileName);
Declare(GdipRecordMetafileFileNameI);
Declare(GdipRecordMetafileStream);
Declare(GdipRecordMetafileStreamI);
Declare(GdipSetMetafileDownLevelRasterizationLimit);
Declare(GdipGetMetafileDownLevelRasterizationLimit);
Declare(GdipGetImageDecodersSize);
Declare(GdipGetImageDecoders);
Declare(GdipGetImageEncodersSize);
Declare(GdipGetImageEncoders);
Declare(GdipComment);
Declare(GdipCreateFontFamilyFromName);
Declare(GdipDeleteFontFamily);
Declare(GdipCloneFontFamily);
Declare(GdipGetGenericFontFamilySansSerif);
Declare(GdipGetGenericFontFamilySerif);
Declare(GdipGetGenericFontFamilyMonospace);
Declare(GdipGetFamilyName);
Declare(GdipIsStyleAvailable);
Declare(GdipFontCollectionEnumerable);
Declare(GdipFontCollectionEnumerate);
Declare(GdipGetEmHeight);
Declare(GdipGetCellAscent);
Declare(GdipGetCellDescent);
Declare(GdipGetLineSpacing);
Declare(GdipCreateFontFromDC);
Declare(GdipCreateFontFromLogfontA);
Declare(GdipCreateFontFromLogfontW);
Declare(GdipCreateFont);
Declare(GdipCloneFont);
Declare(GdipDeleteFont);
Declare(GdipGetFamily);
Declare(GdipGetFontStyle);
Declare(GdipGetFontSize);
Declare(GdipGetFontUnit);
Declare(GdipGetFontHeight);
Declare(GdipGetFontHeightGivenDPI);
Declare(GdipGetLogFontA);
Declare(GdipGetLogFontW);
Declare(GdipNewInstalledFontCollection);
Declare(GdipNewPrivateFontCollection);
Declare(GdipDeletePrivateFontCollection);
Declare(GdipGetFontCollectionFamilyCount);
Declare(GdipGetFontCollectionFamilyList);
Declare(GdipPrivateAddFontFile);
Declare(GdipPrivateAddMemoryFont);
Declare(GdipDrawString);
Declare(GdipMeasureString);
Declare(GdipMeasureCharacterRanges);
Declare(GdipDrawDriverString);
Declare(GdipMeasureDriverString);
Declare(GdipCreateStringFormat);
Declare(GdipStringFormatGetGenericDefault);
Declare(GdipStringFormatGetGenericTypographic);
Declare(GdipDeleteStringFormat);
Declare(GdipCloneStringFormat);
Declare(GdipSetStringFormatFlags);
Declare(GdipGetStringFormatFlags);
Declare(GdipSetStringFormatAlign);
Declare(GdipGetStringFormatAlign);
Declare(GdipSetStringFormatLineAlign);
Declare(GdipGetStringFormatLineAlign);
Declare(GdipSetStringFormatTrimming);
Declare(GdipGetStringFormatTrimming);
Declare(GdipSetStringFormatHotkeyPrefix);
Declare(GdipGetStringFormatHotkeyPrefix);
Declare(GdipSetStringFormatTabStops);
Declare(GdipGetStringFormatTabStops);
Declare(GdipGetStringFormatTabStopCount);
Declare(GdipSetStringFormatDigitSubstitution);
Declare(GdipGetStringFormatDigitSubstitution);
Declare(GdipGetStringFormatMeasurableCharacterRangeCount);
Declare(GdipSetStringFormatMeasurableCharacterRanges);
Declare(GdipCreateCachedBitmap);
Declare(GdipDeleteCachedBitmap);
Declare(GdipDrawCachedBitmap);
Declare(GdipEmfToWmfBits);
Declare(GdipSetImageAttributesCachedBackground);
Declare(GdipTestControl);
Declare(GdiplusNotificationHook);
Declare(GdiplusNotificationUnhook);

#ifdef __cplusplus
}
#endif

#endif // !_FLATAPI_H
