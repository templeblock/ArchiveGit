/******************************************************************************
 **																			 **
 ** 	Module:		AttributeSet_Lib.h										 **
 ** 																		 **
 ** 																		 **
 ** 	Purpose: 	Attribute typed access routines	 						 **
 ** 																		 **
 ** 																		 **
 ** 																		 **
 ** 	Copyright (C) 1992, 1993 Apple Computer, Inc.  All rights reserved.	 **
 ** 																		 **
 **		Change Log:															 **
 **																			 **
 *****************************************************************************/
#ifndef AttributeSet_Lib_h
#define AttributeSet_Lib_h

#include "QD3DSet.h"

#if PRAGMA_ONCE
	#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

/******************************************************************************
 **																			 **
 **							AttributeSet Typed routines						 **
 **																			 **
 *****************************************************************************/

TQ3Status AttributeSet_AddSurfaceUV(
	TQ3AttributeSet			attributeSet,
	const TQ3Param2D			*data);

TQ3Status AttributeSet_AddShadingUV(
	TQ3AttributeSet			attributeSet,
	const TQ3Param2D			*data);

TQ3Status AttributeSet_AddNormal(
	TQ3AttributeSet			attributeSet,
	const TQ3Vector3D		*data);

TQ3Status AttributeSet_AddAmbientCoefficient(
	TQ3AttributeSet			attributeSet,
	const float				*data);

TQ3Status AttributeSet_AddDiffuseColor(
	TQ3AttributeSet			attributeSet,
	const TQ3ColorRGB		*data);

TQ3Status AttributeSet_AddSpecularColor(
	TQ3AttributeSet			attributeSet,
	const TQ3ColorRGB		*data);

TQ3Status AttributeSet_AddSpecularControl(
	TQ3AttributeSet			attributeSet,
	const float				*data);

TQ3Status AttributeSet_AddTransparencyColor(
	TQ3AttributeSet			attributeSet,
	const TQ3ColorRGB		*data);

TQ3Status AttributeSet_AddSurfaceTangent(
	TQ3AttributeSet			attributeSet,
	const TQ3Tangent2D		*data);

TQ3Status AttributeSet_AddSurfaceShader(
	TQ3AttributeSet				attributeSet,
	TQ3SurfaceShaderObject		*surfaceShaderObject);

TQ3Status AttributeSet_GetSurfaceUV(
	TQ3AttributeSet			attributeSet,
	TQ3Param2D				*data);

TQ3Status AttributeSet_GetShadingUV(
	TQ3AttributeSet			attributeSet,
	TQ3Param2D				*data);

TQ3Status AttributeSet_GetNormal(
	TQ3AttributeSet			attributeSet,
	TQ3Vector3D 				*data);

TQ3Status AttributeSet_GetAmbientCoefficient(
	TQ3AttributeSet			attributeSet,
	float					*data);

TQ3Status AttributeSet_GetDiffuseColor(
	TQ3AttributeSet			attributeSet,
	TQ3ColorRGB				*data);

TQ3Status AttributeSet_GetSpecularColor(
	TQ3AttributeSet			attributeSet,
	TQ3ColorRGB				*data);

TQ3Status AttributeSet_GetSpecularControl(
	TQ3AttributeSet			attributeSet,
	float					*data);

TQ3Status AttributeSet_GetTransparencyColor(
	TQ3AttributeSet			attributeSet,
	TQ3ColorRGB				*data);

TQ3Status AttributeSet_GetSurfaceTangent(
	TQ3AttributeSet			attributeSet,
	TQ3Tangent2D				*data);

TQ3Status AttributeSet_GetSurfaceShader(
	TQ3AttributeSet			attributeSet,
	TQ3SurfaceShaderObject	*data);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif  /*  AttributeSet_Lib_h  */
