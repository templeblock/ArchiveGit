/******************************************************************************
 **																			 **
 ** 	Module:		QD3DView.h												 **
 **																			 **
 **																			 **
 ** 	Purpose: 	View types and routines									 **
 **																			 **
 **																			 **
 **																			 **
 ** 	Copyright (C) 1992-1997 Apple Computer, Inc.  All rights reserved.	 **
 **																			 **
 **																			 **
 *****************************************************************************/
#ifndef QD3DView_h
#define QD3DView_h

#include "QD3D.h"
#include "QD3DStyle.h"
#include "QD3DSet.h"

#if defined(PRAGMA_ONCE) && PRAGMA_ONCE
	#pragma once
#endif  /*  PRAGMA_ONCE  */

#if defined(OS_MACINTOSH) && OS_MACINTOSH

#if defined(__xlc__) || defined(__XLC121__)
	#pragma options enum=int
	#pragma options align=power
#elif defined(__MWERKS__)
	#pragma enumsalwaysint on
	#pragma options align=native
#elif defined(__MRC__) || defined(__SC__)
	#if __option(pack_enums)
		#define PRAGMA_ENUM_RESET_QD3DVIEW 1
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
 **						View Type Definitions								 **
 **																			 **
 *****************************************************************************/

typedef enum TQ3ViewStatus {
	kQ3ViewStatusDone,
	kQ3ViewStatusRetraverse,
	kQ3ViewStatusError,
	kQ3ViewStatusCancelled
} TQ3ViewStatus;

#if defined(ESCHER_VER_FUTURE) && ESCHER_VER_FUTURE

typedef struct TQ3RendererCachePrivate *TQ3RendererCache;

#endif /* ESCHER_VER_FUTURE */

/******************************************************************************
 **																			 **
 **						Default Attribute Set								 **
 **																			 **
 *****************************************************************************/

#define kQ3ViewDefaultAmbientCoefficient	1.0
#define kQ3ViewDefaultDiffuseColor			0.5, 0.5, 0.5
#define kQ3ViewDefaultSpecularColor			0.5, 0.5, 0.5
#define kQ3ViewDefaultSpecularControl		4.0
#define kQ3ViewDefaultTransparency			1.0, 1.0, 1.0
#define kQ3ViewDefaultHighlightState		kQ3Off
#define kQ3ViewDefaultHighlightColor		1.0, 0.0, 0.0

#define kQ3ViewDefaultSubdivisionMethod		kQ3SubdivisionMethodScreenSpace
#define kQ3ViewDefaultSubdivisionC1			20.0
#define kQ3ViewDefaultSubdivisionC2			20.0


/******************************************************************************
 **																			 **
 **							View Routines									 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3ViewObject QD3D_CALL Q3View_New(
	void);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_Cancel(
	TQ3ViewObject		view);

#if defined(ESCHER_VER_FUTURE) && ESCHER_VER_FUTURE

QD3D_EXPORT TQ3ObjectType QD3D_CALL Q3View_GetType(
	TQ3ViewObject		view);
	
#endif  /*  ESCHER_VER_FUTURE  */

/******************************************************************************
 **																			 **
 **						View Rendering routines								 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetRendererByType(
	TQ3ViewObject 		view,
	TQ3ObjectType 		type);
	
QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetRenderer(
	TQ3ViewObject 		view,
	TQ3RendererObject	renderer);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetRenderer(
	TQ3ViewObject		view,
	TQ3RendererObject	*renderer);
	
QD3D_EXPORT TQ3Status QD3D_CALL Q3View_StartRendering(
	TQ3ViewObject 		view);
	
QD3D_EXPORT TQ3ViewStatus QD3D_CALL Q3View_EndRendering(
	TQ3ViewObject 		view);
	
QD3D_EXPORT TQ3Status QD3D_CALL Q3View_Flush(
	TQ3ViewObject		view);
	
QD3D_EXPORT TQ3Status QD3D_CALL Q3View_Sync(
	TQ3ViewObject		view);

#if defined(ESCHER_VER_FUTURE) && ESCHER_VER_FUTURE

/******************************************************************************
 **																			 **
 **						View Cache Rendering routines						 **
 **																			 **
 *****************************************************************************/

/*
 *	Q3RendererCache_New
 *	
 *	Create a new, empty rendering cache
 */
QD3D_EXPORT TQ3RendererCache QD3D_CALL Q3RendererCache_New(
	void);

/*
 *	Q3RendererCache_Invalidate
 *	
 *	Deletes any cached data and forces re-traversal in the
 *	rendering loop. (see comment below for rendering loop example)
 */
QD3D_EXPORT TQ3Status QD3D_CALL Q3RendererCache_Invalidate(
	TQ3RendererCache		rendererCache);

/*
 *	Q3RendererCache_Delete
 *	
 *	Delete a rendering cache.
 */
QD3D_EXPORT TQ3Status QD3D_CALL Q3RendererCache_Delete(
	TQ3RendererCache		rendererCache);

/*
 *	Q3View_StartRendererCache
 *	Q3View_EndRendererCache
 *	
 *	Called between Q3View_StartRendering and Q3View_EndRendering.
 *	
 *	Q3View_BeginRendererCache
 *		returns
 *			kQ3True if the cache requires re-traversal, or
 *			kQ3False if the cache is valid.
 *	
 *	The dependencies of a renderer cache are based on the implementation
 *	of a particular renderer. 
 *	
 *	For example, a Z-Buffered renderer may cache the pixels and Z information
 *	for a particular scene, which depends on the lights and camera in a scene.
 *	
 *	A ray tracer may cache the geometry list, and references to each geometry.
 *	
 *	Therefore, coherency in a renderer cache may depend on what you are
 *	editing in a scene.
 *	
 *	You should structure your code as such:
 *	
 *	Q3View_StartRendering(view)
 *	do {
 *		if (Q3View_StartRendererCache(view, cacheID)) {
 *			Q3Group_Submit(largeAuditorium, view);
 *			Q3View_EndRendererCache(view);
 *		}
 *		Q3Group_Submit(peopleMillingAbout, view);
 *	} while (Q3View_EndRendering(view) == kQ3ViewStatusRetraverse);
 *	
 *	The cached information will be regenerated whenever the cache is
 *	invalidated.
 */

QD3D_EXPORT TQ3Boolean QD3D_CALL Q3View_StartRendererCache(
	TQ3ViewObject			view,
	TQ3RendererCache		cacheID);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_EndRendererCache(
	TQ3ViewObject			view);

#endif  /*  ESCHER_VER_FUTURE  */

/******************************************************************************
 **																			 **
 **						View/Bounds/Pick routines							 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_StartBoundingBox(
	TQ3ViewObject		view,
	TQ3ComputeBounds	computeBounds);

QD3D_EXPORT TQ3ViewStatus QD3D_CALL Q3View_EndBoundingBox(
	TQ3ViewObject		view,
	TQ3BoundingBox		*result);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_StartBoundingSphere(
	TQ3ViewObject		view,
	TQ3ComputeBounds	computeBounds);

QD3D_EXPORT TQ3ViewStatus QD3D_CALL Q3View_EndBoundingSphere(
	TQ3ViewObject		view,
	TQ3BoundingSphere	*result);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_StartPicking(
	TQ3ViewObject		view,
	TQ3PickObject		pick);

QD3D_EXPORT TQ3ViewStatus QD3D_CALL Q3View_EndPicking(
	TQ3ViewObject		view);


/******************************************************************************
 **																			 **
 **							View/Camera routines							 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetCamera(
	TQ3ViewObject		view,
	TQ3CameraObject		*camera);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetCamera(
	TQ3ViewObject		view,
	TQ3CameraObject		camera);

#if defined(ESCHER_VER_FUTURE) && ESCHER_VER_FUTURE

#if 0
/* If'ed out so linker doesn't complain */

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetCameraTransformState(
	TQ3ViewObject		view,
	TQ3Boolean			*cameraIsTranformed);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetCameraTransformState(
	TQ3ViewObject		view,
	TQ3Boolean			cameraIsTranformed);

#endif  /*  0  */

#endif  /*  ESCHER_VER_FUTURE  */

/******************************************************************************
 **																			 **
 **							View/Shader routines							 **
 **																			 **
 *****************************************************************************/

#if defined(ESCHER_VER_FUTURE) && ESCHER_VER_FUTURE

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetBackgroundShader(
	TQ3ViewObject		view,
	TQ3ShaderObject		backgroundShader);
	
QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetBackgroundShader(
	TQ3ViewObject		view,
	TQ3ShaderObject		*backgroundShader);

#endif  /*  ESCHER_VER_FUTURE  */


/******************************************************************************
 **																			 **
 **							View/Lights routines							 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetLightGroup(
	TQ3ViewObject		view,
	TQ3GroupObject		lightGroup);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetLightGroup(
	TQ3ViewObject		view,
	TQ3GroupObject		*lightGroup);

#if defined(ESCHER_VER_FUTURE) && ESCHER_VER_FUTURE

#if 0
/* If'ed out so linker doesn't complain */

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetLightTransformState(
	TQ3ViewObject		view,
	TQ3Boolean			*lightsAreTransformed);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetLightTransformState(
	TQ3ViewObject		view,
	TQ3Boolean			lightsAreTransformed);
	
#endif  /*  0  */

#endif  /*  ESCHER_VER_FUTURE  */

/******************************************************************************
 **																			 **
 **								Idle Method									 **
 **																			 **
 *****************************************************************************/
/*
 *	The idle methods allow the application to register callback routines 
 *	which will be called by the view during especially long operations.
 *
 *	The idle methods may also be used to interrupt long renderings or
 *	traversals.  Inside	the idler callback the application can check for
 *	Command-Period, Control-C or clicking a "Cancel" button or whatever else
 *	may be used to let the user interrupt rendering.	
 *
 *	It is NOT LEGAL to call QD3D routines inside an idler callback.
 *
 *	Return kQ3Failure to cancel rendering, kQ3Success to continue. Don't
 *	bother posting an error.
 *
 *	Q3View_SetIdleMethod registers a callback that can be called
 *	by the system during rendering.  Unfortunately there is no way yet
 *	to set timer intervals when you want to be called.  Basically, it is
 *	up to the application's idler callback to check clocks to see if you
 *	were called back only a millisecond ago or an hour ago!
 *
 *	Q3View_SetIdleProgressMethod registers a callback that also gives
 *	progress information. This information is supplied by the renderer, and
 *	may or may not be based on real time.
 *
 *	If a renderer doesn't support the progress method, your method will be
 *	called with current == 0 and completed == 0.
 *	
 *	Otherwise, you are GUARANTEED to get called at least 2 or more times:
 *	
 *	ONCE			idleMethod(view, 0, n)		-> Initialize, Show Dialog
 *	zero or more	idleMethod(view, 1..n-1, n) -> Update progress
 *	ONCE			idleMethod(view, n, n)		-> Exit, Hide Dialog
 *	
 *	"current" is guaranteed to be less than or equal to "completed"
 *	"completed" may change values, but current/complete always indicates
 *	the degree of completion.
 *
 *	The calling conventions aid in managing any data associated with a 
 *	progress user interface indicator.
*/

typedef TQ3Status (QD3D_CALLBACK *TQ3ViewIdleMethod)(
	TQ3ViewObject		view,
	const void			*idlerData);

typedef TQ3Status (QD3D_CALLBACK *TQ3ViewIdleProgressMethod)(
	TQ3ViewObject		view,
	const void			*idlerData,
	unsigned long		current,
	unsigned long		completed);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetIdleMethod(
	TQ3ViewObject		view,
	TQ3ViewIdleMethod	idleMethod,
	const void	 		*idleData);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetIdleProgressMethod(
	TQ3ViewObject				view,
	TQ3ViewIdleProgressMethod	idleMethod,
	const void 					*idleData);

/******************************************************************************
 **																			 **
 **								EndFrame Method								 **
 **																			 **
 *****************************************************************************/

/*
 *	The end frame method is an alternate way of determining when an
 *	asynchronous renderer has completed rendering a frame. It differs from
 *	Q3View_Sync in that notification of the frame completion is the opposite
 *	direction. 
 *	
 *	With Q3View_Sync the application asks a renderer to finish rendering
 *	a frame, and blocks until the frame is complete.
 *	
 *	With the EndFrame method, the renderer tells the application that is has
 *	completed a frame.
 *
 *	If "Q3View_Sync" is called BEFORE this method has been called, this
 *	method will NOT be called ever.
 *	
 *	If "Q3View_Sync" is called AFTER this method has been called, the
 *	call will return immediately (as the frame has already been completed).
 */

typedef void (QD3D_CALLBACK *TQ3ViewEndFrameMethod)(
	TQ3ViewObject			view,
	void					*endFrameData);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetEndFrameMethod(
	TQ3ViewObject			view,
	TQ3ViewEndFrameMethod	endFrame,
	void	 				*endFrameData);

/******************************************************************************
 **																			 **
 **							Push/Pop routines								 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status QD3D_CALL Q3Push_Submit(
	TQ3ViewObject 	view);

QD3D_EXPORT TQ3Status QD3D_CALL Q3Pop_Submit(
	TQ3ViewObject 	view);


/******************************************************************************
 **																			 **
 **		Check if bounding box is visible in the viewing frustum.  Transforms **
 **		the bbox by the current local_to_world transformation matrix and	 **
 **		does a clip test to see if it lies in the viewing frustum.			 **
 **		This can be used by applications to cull out large chunks of scenes	 **
 **		that are not going to be visible.									 **
 **																			 **
 **		The default implementation is to always return kQ3True.  Renderers	 **
 **		may override this routine however to do the checking.				 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Boolean QD3D_CALL Q3View_IsBoundingBoxVisible(
	TQ3ViewObject			view,
	const TQ3BoundingBox	*bbox);


/******************************************************************************
 **																			 **
 **							DrawContext routines							 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetDrawContext(
	TQ3ViewObject 			view,
	TQ3DrawContextObject	drawContext);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetDrawContext(
	TQ3ViewObject 			view,
	TQ3DrawContextObject	*drawContext);


/******************************************************************************
 **																			 **
 **							Graphics State routines							 **
 **																			 **
 ** The graphics state routines can only be called while rendering (ie. in	 **
 ** between calls to start and end rendering calls).  If they are called	 **
 ** outside of a rendering loop, they will return with error.				 **
 **																			 **
 *****************************************************************************/
  
/******************************************************************************
 **																			 **
 **							Transform routines								 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetLocalToWorldMatrixState(
	TQ3ViewObject		view,
	TQ3Matrix4x4		*matrix);
		
QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetWorldToFrustumMatrixState(
	TQ3ViewObject		view,
	TQ3Matrix4x4		*matrix);
		
QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetFrustumToWindowMatrixState(
	TQ3ViewObject		view,
	TQ3Matrix4x4		*matrix);
	

/******************************************************************************
 **																			 **
 **							Style state routines							 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetBackfacingStyleState(
	TQ3ViewObject			view,
	TQ3BackfacingStyle		*backfacingStyle);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetInterpolationStyleState(
	TQ3ViewObject			view,
	TQ3InterpolationStyle	*interpolationType);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetFillStyleState(
	TQ3ViewObject			view,
	TQ3FillStyle			*fillStyle);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetHighlightStyleState(
	TQ3ViewObject			view,
	TQ3AttributeSet			*highlightStyle);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetSubdivisionStyleState(
	TQ3ViewObject			view,
	TQ3SubdivisionStyleData	*subdivisionStyle);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetOrientationStyleState(
	TQ3ViewObject			view,
	TQ3OrientationStyle		*fontFacingDirectionStyle);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetReceiveShadowsStyleState(
	TQ3ViewObject			view,
	TQ3Boolean				*receives);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetPickIDStyleState(
	TQ3ViewObject			view,
	unsigned long			*pickIDStyle);
	
QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetPickPartsStyleState(
	TQ3ViewObject			view,
	TQ3PickParts			*pickPartsStyle);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetAntiAliasStyleState(
	TQ3ViewObject			view,
	TQ3AntiAliasStyleData	*antiAliasData);
	

/******************************************************************************
 **																			 **
 **						Attribute state routines							 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetDefaultAttributeSet(
	TQ3ViewObject		view,
	TQ3AttributeSet		*attributeSet);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetDefaultAttributeSet(
	TQ3ViewObject		view,
	TQ3AttributeSet		attributeSet);


QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetAttributeSetState(
	TQ3ViewObject 		view,
	TQ3AttributeSet		*attributeSet);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetAttributeState(
	TQ3ViewObject 		view,
	TQ3AttributeType	attributeType,
	void				*data);

#if defined(ESCHER_VER_FUTURE) && ESCHER_VER_FUTURE

/******************************************************************************
 **																			 **
 **							Animation routines								 **
 **																			 **
 *****************************************************************************/

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetGlobalTime(
	TQ3ViewObject 		view,
	float				time);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetGlobalTime(
	TQ3ViewObject 		view,
	float				*time);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetAnimationDuration(
	TQ3ViewObject 		view,
	float				duration);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetAnimationDuration(
	TQ3ViewObject 		view,
	float				*duration);

QD3D_EXPORT TQ3Status QD3D_CALL Q3View_SetAnimationEngineState(
	TQ3ViewObject 		view,
	TQ3Switch	 		state);
	
QD3D_EXPORT TQ3Status QD3D_CALL Q3View_GetAnimationEngineState(
	TQ3ViewObject 		view,
	TQ3Switch	 		*state);

#endif  /*  ESCHER_VER_FUTURE  */

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
	#if PRAGMA_ENUM_RESET_QD3DVIEW
		#pragma options(pack_enums)
		#undef PRAGMA_ENUM_RESET_QD3DVIEW
	#endif
	#pragma options align=reset
#endif

#endif  /* OS_MACINTOSH */

#endif  /*  QD3DView_h  */
