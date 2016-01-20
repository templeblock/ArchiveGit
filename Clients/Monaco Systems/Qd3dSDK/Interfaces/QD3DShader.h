/******************************************************************************
 **																			 **
 ** 	Module:		QD3DShader.h											 **
 **																			 **
 **																			 **
 ** 	Purpose: 	QuickDraw 3D Shader / Color Routines					 **
 **																			 **
 **																			 **
 **																			 **
 ** 	Copyright (C) 1991-1997 Apple Computer, Inc. All rights reserved.	 **
 **																			 **
 **																			 **
 *****************************************************************************/
#ifndef QD3DShader_h
#define QD3DShader_h

#include "QD3D.h"

#if defined(PRAGMA_ONCE) && PRAGMA_ONCE
	#pragma once
#endif

#if defined(OS_MACINTOSH) && OS_MACINTOSH

#if defined(__xlc__) || defined(__XLC121__)
	#pragma options enum=int
	#pragma options align=power
#elif defined(__MWERKS__)
	#pragma enumsalwaysint on
	#pragma options align=native
#elif defined(__MRC__) || defined(__SC__)
	#if __option(pack_enums)
		#define PRAGMA_ENUM_RESET_QD3DSHADER 1
	#endif
	#pragma options(!pack_enums)
	#pragma options align=power
#endif

#endif  /* OS_MACINTOSH */

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

/******************************************************************************
 **																			 **
 **								RGB Color routines							 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3ColorRGB *QD3D_CALL Q3ColorRGB_Set(
	TQ3ColorRGB			*color,
	float				r,
	float				g,
	float				b);

QD3D_EXPORT TQ3ColorARGB *QD3D_CALL Q3ColorARGB_Set(
	TQ3ColorARGB		*color,
	float				a,
	float				r,
	float				g,
	float				b);

QD3D_EXPORT TQ3ColorRGB *QD3D_CALL Q3ColorRGB_Add(
	const TQ3ColorRGB 	*c1, 
	const TQ3ColorRGB 	*c2,
	TQ3ColorRGB			*result);

QD3D_EXPORT TQ3ColorRGB *QD3D_CALL Q3ColorRGB_Subtract(
	const TQ3ColorRGB 	*c1, 
	const TQ3ColorRGB 	*c2,
	TQ3ColorRGB			*result);

QD3D_EXPORT TQ3ColorRGB *QD3D_CALL Q3ColorRGB_Scale(
	const TQ3ColorRGB 	*color, 
	float				scale,
	TQ3ColorRGB			*result);

QD3D_EXPORT TQ3ColorRGB *QD3D_CALL Q3ColorRGB_Clamp(
	const TQ3ColorRGB 	*color,
	TQ3ColorRGB			*result);

QD3D_EXPORT TQ3ColorRGB *QD3D_CALL Q3ColorRGB_Lerp(
	const TQ3ColorRGB 	*first, 
	const TQ3ColorRGB 	*last, 
	float 				alpha,
	TQ3ColorRGB 		*result);

QD3D_EXPORT TQ3ColorRGB *QD3D_CALL Q3ColorRGB_Accumulate(
	const TQ3ColorRGB 	*src, 
	TQ3ColorRGB 		*result);

QD3D_EXPORT float *QD3D_CALL Q3ColorRGB_Luminance(
	const TQ3ColorRGB	*color, 
	float 				*luminance);


/******************************************************************************
 **																			 **
 **								Shader Types								 **
 **																			 **
 *****************************************************************************/

typedef enum TQ3ShaderUVBoundary {
	kQ3ShaderUVBoundaryWrap,
	kQ3ShaderUVBoundaryClamp
} TQ3ShaderUVBoundary;


/******************************************************************************
 **																			 **
 **								Shader Routines								 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3ObjectType QD3D_CALL Q3Shader_GetType(
	TQ3ShaderObject			shader);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Shader_Submit(
	TQ3ShaderObject			shader, 
	TQ3ViewObject			view);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Shader_SetUVTransform(
	TQ3ShaderObject			shader,
	const TQ3Matrix3x3		*uvTransform);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Shader_GetUVTransform(
	TQ3ShaderObject			shader,
	TQ3Matrix3x3			*uvTransform);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Shader_SetUBoundary(
	TQ3ShaderObject			shader,
	TQ3ShaderUVBoundary		uBoundary);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Shader_SetVBoundary(
	TQ3ShaderObject			shader,
	TQ3ShaderUVBoundary		vBoundary);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Shader_GetUBoundary(
	TQ3ShaderObject			shader,
	TQ3ShaderUVBoundary		*uBoundary);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Shader_GetVBoundary(
	TQ3ShaderObject			shader,
	TQ3ShaderUVBoundary		*vBoundary);


/******************************************************************************
 **																			 **
 **							Illumination Shader	Classes						 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3ObjectType QD3D_CALL Q3IlluminationShader_GetType(
	TQ3ShaderObject				shader);

QD3D_EXPORT TQ3ShaderObject QD3D_CALL Q3PhongIllumination_New(
	void);

QD3D_EXPORT TQ3ShaderObject QD3D_CALL Q3LambertIllumination_New(
	void);

QD3D_EXPORT TQ3ShaderObject QD3D_CALL Q3NULLIllumination_New(
	void);


/******************************************************************************
 **																			 **
 **								 Surface Shader								 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3ObjectType QD3D_CALL Q3SurfaceShader_GetType(
	TQ3SurfaceShaderObject		shader);


/******************************************************************************
 **																			 **
 **								Texture Shader								 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3ShaderObject QD3D_CALL Q3TextureShader_New(
	TQ3TextureObject			texture);

QD3D_EXPORT TQ3Status QD3D_CALL Q3TextureShader_GetTexture(
	TQ3ShaderObject				shader,
	TQ3TextureObject			*texture);

QD3D_EXPORT TQ3Status QD3D_CALL Q3TextureShader_SetTexture(
	TQ3ShaderObject				shader,
	TQ3TextureObject			texture);


/******************************************************************************
 **																			 **
 **								Texture Objects								 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3ObjectType QD3D_CALL Q3Texture_GetType(
	TQ3TextureObject		texture);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Texture_GetWidth(
	TQ3TextureObject		texture,
	unsigned long			*width);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Texture_GetHeight(
	TQ3TextureObject		texture,
	unsigned long			*height);


/******************************************************************************
 **																			 **
 **								Pixmap Texture							     **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3TextureObject QD3D_CALL Q3PixmapTexture_New(
	const TQ3StoragePixmap	*pixmap);

QD3D_EXPORT TQ3Status QD3D_CALL Q3PixmapTexture_GetPixmap(
	TQ3TextureObject		texture,
	TQ3StoragePixmap		*pixmap);

QD3D_EXPORT TQ3Status QD3D_CALL Q3PixmapTexture_SetPixmap(
	TQ3TextureObject		texture,
	const TQ3StoragePixmap	*pixmap);


/******************************************************************************
 **																			 **
 **								Mipmap Texture							     **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3TextureObject QD3D_CALL Q3MipmapTexture_New(
	const TQ3Mipmap			*mipmap);

QD3D_EXPORT TQ3Status QD3D_CALL Q3MipmapTexture_GetMipmap(
	TQ3TextureObject		texture,
	TQ3Mipmap				*mipmap);

QD3D_EXPORT TQ3Status QD3D_CALL Q3MipmapTexture_SetMipmap(
	TQ3TextureObject		texture,
	const TQ3Mipmap			*mipmap);


#if defined(ESCHER_VER_FUTURE) && ESCHER_VER_FUTURE

/******************************************************************************
 **																			 **
 **		Whoa! {neigh!}													     **
 **																			 **
 *****************************************************************************/

#define kQ3NeighborhoodFlagImagePlane					((unsigned long)1 << 0)
#define kQ3NeighborhoodFlagViewInfo						((unsigned long)1 << 1)
#define kQ3NeighborhoodFlagLightInfo					((unsigned long)1 << 2)
#define kQ3NeighborhoodFlagRenderer						((unsigned long)1 << 3)

#define kQ3NeighborhoodFlagAmbientCoefficient			((unsigned long)1 << 4)
#define kQ3NeighborhoodFlagDiffuseColor					((unsigned long)1 << 5)
#define kQ3NeighborhoodFlagSpecularColor				((unsigned long)1 << 6)
#define kQ3NeighborhoodFlagSpecularControl				((unsigned long)1 << 7)
#define kQ3NeighborhoodFlagTransparencyColor			((unsigned long)1 << 8)
#define kQ3NeighborhoodFlagEmittedColor					((unsigned long)1 << 9)

#define kQ3NeighborhoodFlagDepth						((unsigned long)1 << 10)

#define kQ3NeighborhoodFlagSurfaceUV					((unsigned long)1 << 11)
#define kQ3NeighborhoodFlagSurfaceLocation				((unsigned long)1 << 12)
#define kQ3NeighborhoodFlagSurfaceNormal				((unsigned long)1 << 13)
#define kQ3NeighborhoodFlagSurfaceTangent				((unsigned long)1 << 14)

#define kQ3NeighborhoodFlagSurfaceFilterSize			((unsigned long)1 << 15)	
#define kQ3NeighborhoodFlagSurfaceProjectedPixelX		((unsigned long)1 << 16)
#define kQ3NeighborhoodFlagSurfaceProjectedPixelY		((unsigned long)1 << 17)

#define kQ3NeighborhoodFlagSurfaceWorldFilterSize		((unsigned long)1 << 18)	
#define kQ3NeighborhoodFlagSurfaceWorldProjectedPixelX	((unsigned long)1 << 19)
#define kQ3NeighborhoodFlagSurfaceWorldProjectedPixelY	((unsigned long)1 << 20)

#define kQ3NeighborhoodFlagShadingUV					((unsigned long)1 << 21)
#define kQ3NeighborhoodFlagShadingNormal				((unsigned long)1 << 22)
#define kQ3NeighborhoodFlagShadingColor					((unsigned long)1 << 23)
#define kQ3NeighborhoodFlagShadingScalar				((unsigned long)1 << 24)
#define kQ3NeighborhoodFlagShadingOpacity				((unsigned long)1 << 25)
#define kQ3NeighborhoodFlagShadingTrim					((unsigned long)1 << 26)

typedef struct TQ3ImageSubRegion {
	unsigned long 		north;
	unsigned long		east;
	
	unsigned long 		south;
	unsigned long 		west;
} TQ3ImageSubRegion;

typedef struct TQ3ImagePlane {
	/* pixel location */
	unsigned long		imageX;
	unsigned long		imageY;
	
	/* imageplane size */
	unsigned long		imageSizeX;
	unsigned long		imageSizeY;
	
	/* pixel location mapped to 0-1 */
	TQ3Param2D			imageUV;

	/* valid sub-region of pixmap */	
	TQ3ImageSubRegion	imageSubRegion;
	 
	/* pixmap typically attached to drawcontext */
	TQ3Pixmap			pixmap;
} TQ3ImagePlane;

typedef struct TQ3ViewInfo {
	TQ3CameraObject		camera;
	TQ3Point3D			cameraLocation;
	TQ3Vector3D			viewDirection;
} TQ3ViewInfo;

typedef struct TQ3NeighborhoodPrivate *TQ3NeighborhoodObject;


/******************************************************************************
 **																			 **
 **							Neighborhood Functions							 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3NeighborhoodObject QD3D_CALL Q3Neighborhood_New(
	void);
	
QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_Dispose(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_Initialize(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_Validate(
	TQ3NeighborhoodObject	neigh,
	unsigned long			field);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_InValidate(
	TQ3NeighborhoodObject	neigh,
	unsigned long			field);

QD3D_EXPORT TQ3Boolean QD3D_CALL Q3Neighborhood_IsValid(
	TQ3NeighborhoodObject	neigh,
	unsigned long			field);

QD3D_EXPORT unsigned long QD3D_CALL Q3Neighborhood_GetValidFlag(
	TQ3NeighborhoodObject	neigh);


/******************************************************************************
 **																			 **
 **								Set Methods									 **
 **																			 **
 *****************************************************************************/
 
QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetImagePlane(
	TQ3NeighborhoodObject	neigh,
	const TQ3ImagePlane		*imagePlane);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetViewInfo(
	TQ3NeighborhoodObject	neigh,
	const TQ3ViewInfo		*viewInfo);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetLightGroup(
	TQ3NeighborhoodObject	neigh,
	TQ3GroupObject			lightGroup);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetRenderer(
	TQ3NeighborhoodObject	neigh,
	TQ3RendererObject		renderer);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetAmbientCoefficient(
	TQ3NeighborhoodObject	neigh,
	float					ambientCoefficient);
	
QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetDiffuseColor(
	TQ3NeighborhoodObject	neigh,
	const TQ3ColorRGB		*diffuseColor);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetSpecularColor(
	TQ3NeighborhoodObject	neigh,
	const TQ3ColorRGB		*specularColor);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetSpecularControl(
	TQ3NeighborhoodObject	neigh,
	float					specularControl);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetTransparencyColor(
	TQ3NeighborhoodObject	neigh,
	const TQ3ColorRGB 		*transparencyColor);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetEmittedColor(
	TQ3NeighborhoodObject	neigh,
	const TQ3ColorRGB		*emittedColor);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetDepth(
	TQ3NeighborhoodObject	neigh,
	float					depth);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetSurfaceUV(
	TQ3NeighborhoodObject	neigh,
	const TQ3Param2D		*surfaceUV);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetSurfaceLocation(
	TQ3NeighborhoodObject	neigh,
	const TQ3Point3D		*surfaceLocation);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetSurfaceNormal(
	TQ3NeighborhoodObject	neigh,
	const TQ3Vector3D 		*surfaceNormal);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetSurfaceTangent(
	TQ3NeighborhoodObject	neigh,
	const TQ3Tangent2D		*surfaceTangent);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetSurfaceFilterSize(
	TQ3NeighborhoodObject	neigh,
	float					surfaceFilterSize);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetSurfaceProjectedPixelX(
	TQ3NeighborhoodObject	neigh,
	const TQ3Vector2D		*surfaceProjectedPixelX);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetSurfaceProjectedPixelY(
	TQ3NeighborhoodObject	neigh,
	const TQ3Vector2D		*surfaceProjectedPixelY);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetSurfaceWorldFilterSize(
	TQ3NeighborhoodObject	neigh,
	float					surfaceWorldFilterSize);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetSurfaceWorldProjectedPixelX(
	TQ3NeighborhoodObject	neigh,
	const TQ3Vector3D		*surfaceWorldProjectedPixelX);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetSurfaceWorldProjectedPixelY(
	TQ3NeighborhoodObject	neigh,
	const TQ3Vector3D		*surfaceWorldProjectedPixelY);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetShadingUV(
	TQ3NeighborhoodObject	neigh,
	const TQ3Param2D		*shadingUV);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetShadingNormal(
	TQ3NeighborhoodObject	neigh,
	const TQ3Vector3D		*shadingNormal);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetShadingColor(
	TQ3NeighborhoodObject	neigh,
	const TQ3ColorRGB		*shadingColor);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetShadingScalar(
	TQ3NeighborhoodObject	neigh,
	float					shadingScalar);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetShadingOpacity(
	TQ3NeighborhoodObject	neigh,
	float					shadingOpacity);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Neighborhood_SetShadingTrim(
	TQ3NeighborhoodObject	neigh,
	TQ3Boolean				shadingTrim);


/******************************************************************************
 **																			 **
 **								Get Methods									 **
 **																			 **
 *****************************************************************************/
 
QD3D_EXPORT TQ3ImagePlane *QD3D_CALL Q3Neighborhood_GetImagePlane(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3ViewInfo *QD3D_CALL Q3Neighborhood_GetViewInfo(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3GroupObject QD3D_CALL Q3Neighborhood_GetLightGroup(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3RendererObject QD3D_CALL Q3Neighborhood_GetRenderer(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT float QD3D_CALL Q3Neighborhood_GetAmbientCoefficient(
	TQ3NeighborhoodObject	neigh);
	
QD3D_EXPORT TQ3ColorRGB *QD3D_CALL Q3Neighborhood_GetDiffuseColor(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3ColorRGB *QD3D_CALL Q3Neighborhood_GetSpecularColor(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT float QD3D_CALL Q3Neighborhood_GetSpecularControl(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3ColorRGB *QD3D_CALL Q3Neighborhood_GetTransparencyColor(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3ColorRGB *QD3D_CALL Q3Neighborhood_GetEmittedColor(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT float QD3D_CALL Q3Neighborhood_GetDepth(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Param2D *QD3D_CALL Q3Neighborhood_GetSurfaceUV(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Point3D *QD3D_CALL Q3Neighborhood_GetSurfaceLocation(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Vector3D *QD3D_CALL Q3Neighborhood_GetSurfaceNormal(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Tangent2D *QD3D_CALL Q3Neighborhood_GetSurfaceTangent(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT float QD3D_CALL Q3Neighborhood_GetSurfaceFilterSize(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Vector2D *QD3D_CALL Q3Neighborhood_GetSurfaceProjectedPixelX(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Vector2D *QD3D_CALL Q3Neighborhood_GetSurfaceProjectedPixelY(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT float QD3D_CALL Q3Neighborhood_GetSurfaceWorldFilterSize(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Vector3D *QD3D_CALL Q3Neighborhood_GetSurfaceWorldProjectedPixelX(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Vector3D *QD3D_CALL Q3Neighborhood_GetSurfaceWorldProjectedPixelY(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Param2D *QD3D_CALL Q3Neighborhood_GetShadingUV(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Vector3D *QD3D_CALL Q3Neighborhood_GetShadingNormal(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3ColorRGB *QD3D_CALL Q3Neighborhood_GetShadingColor(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT float QD3D_CALL Q3Neighborhood_GetShadingScalar(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT float QD3D_CALL Q3Neighborhood_GetShadingOpacity(
	TQ3NeighborhoodObject	neigh);

QD3D_EXPORT TQ3Boolean QD3D_CALL Q3Neighborhood_GetShadingTrim(
	TQ3NeighborhoodObject	neigh);
	
	
/******************************************************************************
 **																			 **
 **							Enum Definitions								 **
 **																			 **
 *****************************************************************************/

typedef enum TQ3ShaderEffect {
	kQ3ShaderEffectReplace,
	kQ3ShaderEffectDarken,
	kQ3ShaderEffectLighten,
	kQ3ShaderEffectMultiply
} TQ3ShaderEffect;


/******************************************************************************
 **																			 **
 **								Shader methods								 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status QD3D_CALL Q3Shader_Evaluate(
	TQ3ShaderObject			shader, 
	TQ3NeighborhoodObject	neigh);

/******************************************************************************
 **																			 **
 **							Class managment methods							 **
 **																			 **
 *****************************************************************************/

/******************************************************************************
 **																			 **
 **							Atmospheric Shader								 **
 **																			 **
 *****************************************************************************/

#if 0
/* If'ed out so linker doesn't complain */

QD3D_EXPORT TQ3ShaderObject QD3D_CALL Q3AtmosphericShader_New(
	void);
	
QD3D_EXPORT TQ3Status QD3D_CALL Q3AtmosphericShader_GetExtinctionCoefficient(
	TQ3ShaderObject			shader,
	float					*extinctionCoefficient);

QD3D_EXPORT TQ3Status QD3D_CALL Q3AtmosphericShader_GetAtmosphereColor(
	TQ3ShaderObject			shader,
	TQ3ColorRGB				*atmosphereColor);

QD3D_EXPORT TQ3Status QD3D_CALL Q3AtmosphericShader_SetExtinctionCoefficient(
	TQ3ShaderObject			shader,
	float					extinctionCoefficient);

QD3D_EXPORT TQ3Status QD3D_CALL Q3AtmosphericShader_SetAtmosphereColor(
	TQ3ShaderObject			shader,
	const TQ3ColorRGB		*atmosphereColor);

#endif  /*  0  */

/******************************************************************************
 **																			 **
 **						Base Class Illumination Shader						 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status  QD3D_CALL Q3IlluminationShader_SetLightGroup(
	TQ3ShaderObject				shader,
	TQ3GroupObject				lightGroup);

QD3D_EXPORT TQ3Status QD3D_CALL Q3IlluminationShader_GetLightGroup(
	TQ3ShaderObject				shader,
	TQ3GroupObject				*lightGroup);


/******************************************************************************
 **																			 **
 **								Image Shader								 **
 **																			 **
 *****************************************************************************/
 
#define	kQ3ImageShaderForeground	((unsigned long)1 << 1)
#define	kQ3ImageShaderBackground	((unsigned long)1 << 2)

#if 0
/* If'ed out so linker doesn't complain */

QD3D_EXPORT TQ3ShaderObject QD3D_CALL Q3ImageShader_New(
	unsigned long 				kind);

QD3D_EXPORT TQ3Status QD3D_CALL Q3ImageShader_GetSourceShader(
	TQ3ShaderObject				shader,
	TQ3ShaderObject				*sourceShader);

QD3D_EXPORT TQ3Status QD3D_CALL Q3ImageShader_SetSourceShader(
	TQ3ShaderObject				shader,
	const TQ3ShaderObject		sourceShader);


/******************************************************************************
 **																			 **
 **								Trim Shader									 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3ShaderObject QD3D_CALL Q3TrimShader_New(
	void);

QD3D_EXPORT TQ3Status QD3D_CALL Q3TrimShader_GetThreshold(
	TQ3ShaderObject			shader,
	float					*threshold);
	
QD3D_EXPORT TQ3Status QD3D_CALL Q3TrimShader_GetTrimmer(
	TQ3ShaderObject			shader,
	TQ3ShaderObject			*trimmer);

QD3D_EXPORT TQ3Status QD3D_CALL Q3TrimShader_SetThreshold(
	TQ3ShaderObject			shader,
	float					threshold);

QD3D_EXPORT TQ3Status QD3D_CALL Q3TrimShader_SetTrimmer(
	TQ3ShaderObject			shader,
	TQ3ShaderObject			trimmer);

/******************************************************************************
 **																			 **
 **							Attachment Objects								 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status QD3D_CALL Q3Attachment_Evaluate(
	TQ3AttachmentObject			attachment, 
	TQ3NeighborhoodObject		neigh);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Attachment_SetBoundingSphere(
	TQ3AttachmentObject			attachment,
	const TQ3BoundingSphere		*boundingSphere);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Attachment_GetBoundingSphere(
	TQ3AttachmentObject			attachment,
	TQ3BoundingSphere			*boundingSphere);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Attachment_SetAttachmentTransform(
	TQ3AttachmentObject			attachment,
	const TQ3Matrix4x4			*attachmentTransform);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Attachment_GetAttachmentTransform(
	TQ3AttachmentObject			attachment,
	TQ3Matrix4x4				*attachmentTransform);

#endif  /*  0  */


#endif /* ESCHER_VER_FUTURE */

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#if defined(OS_MACINTOSH) && OS_MACINTOSH

#if defined(__xlc__) || defined(__XLC121__)
	#pragma options enum=reset
	#pragma options align=reset
#elif defined(__MWERKS__)
	#pragma enumsalwaysint reset
	#pragma options align=reset
#elif defined(__MRC__) || defined(__SC__)
	#if PRAGMA_ENUM_RESET_QD3DSHADER
		#pragma options(pack_enums)
		#undef PRAGMA_ENUM_RESET_QD3DSHADER
	#endif
	#pragma options align=reset
#endif

#endif  /* OS_MACINTOSH */

#endif  /*  QD3DShader_h  */
