// Box3dSupport.c - QuickDraw 3d routines
//
// This file contains utility routines for QuickDraw 3d sample code.
// This is a simple QuickDraw 3d application to draw a cube in the center 
// of the main application window.  The routines in here handle setting up
// the main display group, the view, the Macintosh 3D draw context, and the
// camera and lighting. 
//
// This code is the basis of the introductory article in  d e v e l o p  issue 22
//
// Nick Thompson - January 6th 1995
// ©1994-95 Apple computer Inc., All Rights Reserved
// 
//
#include "Box3DSupport.h"

#define ErMath_Atan(x)	((float)atan((double)(x)))
#define	kEPSILON		1.19209290e-07


static 	TQ3Point3D	documentGroupCenter;
static	float		documentGroupScale;


TQ3ViewObject MyNewView(DocumentPtr theDocument)
{
	TQ3Status				myStatus;
	TQ3ViewObject			myView;
	TQ3DrawContextObject	myDrawContext;
	TQ3RendererObject		myRenderer;
	TQ3CameraObject			myCamera;
	TQ3GroupObject			myLights;
	
	myView = Q3View_New();
	
	//	Create and set draw context.
	switch( theDocument->drawcontextType ) 
	{
	case kQ3DrawContextTypeWin32DC:
		if ((myDrawContext = NewWin32DCDrawContext(theDocument)) == NULL )
			goto bail;
		break;
 	case kQ3DrawContextTypeDDSurface:
		goto bail;	/* TBD */
	
	default:
	case kQ3DrawContextTypePixmap:
		if ((myDrawContext = NewPixmapDrawContext(theDocument, theDocument->fPixelFormat )) == NULL )
			goto bail;
		break;
	}
		
	if ((myStatus = Q3View_SetDrawContext(myView, myDrawContext)) == kQ3Failure )
		goto bail;

	Q3Object_Dispose( myDrawContext ) ;
	
	//	Create and set renderer.
	switch( theDocument->rendererType ) 
	{
	case kQ3RendererTypeWireFrame: // this uses the wire frame renderer
		myRenderer = Q3Renderer_NewFromType(kQ3RendererTypeWireFrame);
		if ((myStatus = Q3View_SetRenderer(myView, myRenderer)) == kQ3Failure ) {
			goto bail;
		}
		break;
	default:
	case kQ3RendererTypeInteractive: // this uses the interactive renderer
		if ((myRenderer = Q3Renderer_NewFromType(kQ3RendererTypeInteractive)) != NULL ) {
			if ((myStatus = Q3View_SetRenderer(myView, myRenderer)) == kQ3Failure ) {
				goto bail;
			}
			// these two lines set us up to use the best possible renderer,
			// including  hardware if it is installed.
			Q3InteractiveRenderer_SetDoubleBufferBypass (myRenderer, kQ3True);						
			Q3InteractiveRenderer_SetPreferences(myRenderer, kQAVendor_BestChoice, 0);
		}
		else {
			goto bail;
		}
		break;
	}

	Q3Object_Dispose( myRenderer ) ;
	
	//	Create and set camera
	if ( (myCamera = MyNewCamera(theDocument)) == NULL )
		goto bail;
		
	if ((myStatus = Q3View_SetCamera(myView, myCamera)) == kQ3Failure )
		goto bail;

	Q3Object_Dispose( myCamera ) ;
	
	//	Create and set lights
	if ((myLights = MyNewLights()) == NULL )
		goto bail;
		
	if ((myStatus = Q3View_SetLightGroup(myView, myLights)) == kQ3Failure )
		goto bail;
		
	Q3Object_Dispose(myLights);

	return ( myView );
	
bail:
	//	If any of the above failed, then don't return a view.
	return ( NULL );
}

TQ3Status MyResizeView( DocumentPtr theDocument,unsigned long width, unsigned long height )
{
	BOOL		result;
	TQ3Status	aStatus = kQ3Success;
	if( theDocument->fWidth == width &&
	   	theDocument->fHeight == height )
		return aStatus;

	theDocument->fWidth = width;
	theDocument->fHeight = height;
	if( kQ3DrawContextTypePixmap == theDocument->drawcontextType)
	{
		Q3Object_Dispose(theDocument->fView);
		result = DeleteObject(theDocument->fBitmap);
		theDocument->fBitmap = NULL;
		result = DeleteDC(theDocument->fMemoryDC);
		theDocument->fMemoryDC = NULL;
		theDocument->fView = MyNewView(theDocument);
	}
	return aStatus;
}

TQ3Status MyDeleteView( DocumentPtr theDocument )
{
	BOOL		result;
	TQ3Status	aStatus = kQ3Success;

	if( kQ3DrawContextTypePixmap == theDocument->drawcontextType)
	{
		result = DeleteObject(theDocument->fBitmap);
		result = DeleteDC(theDocument->fMemoryDC);
	}

	Q3Object_Dispose(theDocument->fView);
	return aStatus;
}

//----------------------------------------------------------------------------------

TQ3CameraObject MyNewCamera(DocumentPtr theDocument)
{
	TQ3ViewAngleAspectCameraData	perspectiveData;

	TQ3CameraObject				camera;
	
	TQ3Point3D 					from 	= { 0.0F, 0.0F, 7.0F };
	TQ3Point3D 					to 		= { 0.0F, 0.0F, 0.0F };
	TQ3Vector3D 				up 		= { 0.0F, 1.0F, 0.0F };

	float 						fieldOfView = 1.0F;
	float 						hither 		= 0.001F;
	float 						yon 		= 1000.0F;

	TQ3Status					returnVal = kQ3Failure ;

	perspectiveData.cameraData.placement.cameraLocation 	= from;
	perspectiveData.cameraData.placement.pointOfInterest 	= to;
	perspectiveData.cameraData.placement.upVector 			= up;

	perspectiveData.cameraData.range.hither	= hither;
	perspectiveData.cameraData.range.yon 	= yon;

	perspectiveData.cameraData.viewPort.origin.x = -1.0F;
	perspectiveData.cameraData.viewPort.origin.y = 1.0F;
	perspectiveData.cameraData.viewPort.width = 2.0F;
	perspectiveData.cameraData.viewPort.height = 2.0F;
													
	perspectiveData.fov				= fieldOfView;
	perspectiveData.aspectRatioXToY	=
		(float) (theDocument->fWidth) / 
		(float) (theDocument->fHeight);
		
	camera = Q3ViewAngleAspectCamera_New(&perspectiveData);

	return camera ;
}


//----------------------------------------------------------------------------------

TQ3GroupObject MyNewLights()
{
	TQ3GroupPosition		myGroupPosition;
	TQ3GroupObject			myLightList;
	TQ3LightData			myLightData;
	TQ3PointLightData		myPointLightData;
	TQ3DirectionalLightData	myDirectionalLightData;
	TQ3LightObject			myAmbientLight, myPointLight, myFillLight;
	TQ3Point3D				pointLocation = { -10.0F, 0.0F, 10.0F };
	TQ3Vector3D				fillDirection = { 10.0F, 0.0F, 10.0F };
	TQ3ColorRGB				WhiteLight = { 1.0F, 1.0F, 1.0F };
	
	//	Set up light data for ambient light.  This light data will be used for point and fill
	//	light also.

	myLightData.isOn = kQ3True;
	myLightData.color = WhiteLight;
	
	//	Create ambient light.
	myLightData.brightness = .25F;
	myAmbientLight = Q3AmbientLight_New(&myLightData);
	if ( myAmbientLight == NULL )
		goto bail;
	
	//	Create point light.
	myLightData.brightness = 1.0F;
	myPointLightData.lightData = myLightData;
	myPointLightData.castsShadows = kQ3False;
	myPointLightData.attenuation = kQ3AttenuationTypeNone;
	myPointLightData.location = pointLocation;
	myPointLight = Q3PointLight_New(&myPointLightData);
	if ( myPointLight == NULL )
		goto bail;

	//	Create fill light.
	myLightData.brightness = .3F;
	myDirectionalLightData.lightData = myLightData;
	myDirectionalLightData.castsShadows = kQ3False;
	myDirectionalLightData.direction = fillDirection;
	myFillLight = Q3DirectionalLight_New(&myDirectionalLightData);
	if ( myFillLight == NULL )
		goto bail;

	//	Create light group and add each of the lights into the group.
	myLightList = Q3LightGroup_New();
	if ( myLightList == NULL )
		goto bail;
	myGroupPosition = Q3Group_AddObject(myLightList, myAmbientLight);
	if ( myGroupPosition == 0 )
		goto bail;
	myGroupPosition = Q3Group_AddObject(myLightList, myPointLight);
	if ( myGroupPosition == 0 )
		goto bail;
	myGroupPosition = Q3Group_AddObject(myLightList, myFillLight);
	if ( myGroupPosition == 0 )
		goto bail;

	Q3Object_Dispose( myAmbientLight ) ;
	Q3Object_Dispose( myPointLight ) ;
	Q3Object_Dispose( myFillLight ) ;

	//	Done!
	return ( myLightList );
	
bail:
	//	If any of the above failed, then return nothing!
	return ( NULL );
}




static void MyColorBoxFaces( TQ3BoxData *myBoxData )
{
	TQ3ColorRGB				faceColor ;
	short 					face ;
	
	// sanity check - you need to have set up 
	// the face attribute set for the box data 
	// before calling this.
	
	if( myBoxData->faceAttributeSet == NULL )
		return ;
		
	// make each face of a box a different color
	
	for( face = 0; face < 6; face++) {
		
		myBoxData->faceAttributeSet[face] = Q3AttributeSet_New();
		switch( face ) {
			case 0:
				faceColor.r = 1.0F;
				faceColor.g = 0.0F;
				faceColor.b = 0.0F;
				break;
			
			case 1:
				faceColor.r = 0.0F;
				faceColor.g = 1.0F;
				faceColor.b = 0.0F;
				break;
			
			case 2:
				faceColor.r = 0.0F;
				faceColor.g = 0.0F;
				faceColor.b = 1.0F;
				break;
			
			case 3:
				faceColor.r = 1.0F;
				faceColor.g = 1.0F;
				faceColor.b = 0.0F;
				break;
			
			case 4:
				faceColor.r = 1.0F;
				faceColor.g = 0.0F;
				faceColor.b = 1.0F;
				break;
			
			case 5:
				faceColor.r = 0.0F;
				faceColor.g = 1.0F;
				faceColor.b = 1.0F;
				break;
		}
		Q3AttributeSet_Add(myBoxData->faceAttributeSet[face], kQ3AttributeTypeDiffuseColor, &faceColor);
	}
}

static TQ3GroupPosition MyAddTransformedObjectToGroup( TQ3GroupObject theGroup, TQ3Object theObject, TQ3Vector3D *translation )
{
	TQ3TransformObject	transform;

	transform = Q3TranslateTransform_New(translation);
	Q3Group_AddObject(theGroup, transform);	
	Q3Object_Dispose(transform);
	return Q3Group_AddObject(theGroup, theObject);	
}


TQ3GroupObject MyNewModel()
{
	TQ3GroupObject			myGroup = NULL;
	TQ3GeometryObject		myBox;
	TQ3BoxData				myBoxData;
	TQ3ShaderObject			myIlluminationShader ;
	
	TQ3SetObject			faces[6] ;
	short					face ;
			
	// Create a group for the complete model.
	// do not use Q3OrderedDisplayGroup_New since in this
	// type of group all of the translations are applied before
	// the objects in the group are drawn, in this instance we 
	// dont want this.
	if ((myGroup = Q3DisplayGroup_New()) != NULL ) 
	{
			
		// Define a shading type for the group
		// and add the shader to the group
		myIlluminationShader = Q3PhongIllumination_New();
		Q3Group_AddObject(myGroup, myIlluminationShader);

		// set up the colored faces for the box data
		myBoxData.faceAttributeSet = faces;
		myBoxData.boxAttributeSet = NULL;
		MyColorBoxFaces( &myBoxData ) ;
		
		#define	kBoxSide		0.8F
		#define	kBoxSidePlusGap	0.1F

		// create the box itself
		Q3Point3D_Set(&myBoxData.origin, 0.0F, 0.0F, 0.0F);
		Q3Vector3D_Set(&myBoxData.orientation, 0.0F, kBoxSide, 0.0F);
		Q3Vector3D_Set(&myBoxData.majorAxis, 0.0F, 0.0F, kBoxSide);	
		Q3Vector3D_Set(&myBoxData.minorAxis, kBoxSide, 0.0F, 0.0F);	
		myBox = Q3Box_New(&myBoxData);
#if 0	// one box
		Q3Group_AddObject(myGroup, myBox);
#else	// 4 boxes
		{
			TQ3Vector3D				translation;
			translation.x =  0.0F;				
			translation.y = kBoxSidePlusGap; 
			translation.z =  0.0F;
			MyAddTransformedObjectToGroup( myGroup, myBox, &translation ) ;
			translation.x =  2 * kBoxSide;	
			translation.y = kBoxSidePlusGap; 
			translation.z =  0.0F;
			MyAddTransformedObjectToGroup( myGroup, myBox, &translation ) ;
			translation.x =  0.0F;				
			translation.y = kBoxSidePlusGap; 
			translation.z = -2 * kBoxSide;
			MyAddTransformedObjectToGroup( myGroup, myBox, &translation ) ;
			translation.x = -2 * kBoxSide;	
			translation.y = kBoxSidePlusGap; 
			translation.z =  0.0F;
			MyAddTransformedObjectToGroup( myGroup, myBox, &translation ) ;
		}
#endif
		for( face = 0; face < 6; face++) {
			if( myBoxData.faceAttributeSet[face] != NULL )
				Q3Object_Dispose(myBoxData.faceAttributeSet[face]);
		}
		
		if( myBox ) 
			Q3Object_Dispose( myBox );
	}
	
	// dispose of the objects we created here
	if( myIlluminationShader ) 
		Q3Object_Dispose(myIlluminationShader);	
			
	
	//	Done!
	return ( myGroup );
}

void pvCamera_Fit(DocumentPtr theDocument)
{	
	TQ3Point3D 		from, to;
	TQ3BoundingBox	viewBBox;
	float			fieldOfView, hither, yon;

	if (!theDocument)
		return;

	if (!theDocument->fModel)
		return;

	pvBBox_Get(theDocument, &viewBBox);
	pvBBoxCenter(&viewBBox, &to);
	

	{
		TQ3Vector3D viewVector;
		TQ3Vector3D	normViewVector;
		TQ3Vector3D	eyeToFrontClip;
		TQ3Vector3D	eyeToBackClip;
 		TQ3Vector3D	diagonalVector;
		float		viewDistance;
		float 		maxDimension;

		Q3Point3D_Subtract(&viewBBox.max,
						   &viewBBox.min,
						   &diagonalVector);
		maxDimension = Q3Vector3D_Length(&diagonalVector);
		if (maxDimension == 0.0F)
			maxDimension = 1.0F;

		maxDimension *= 8.0F / 7.0F;
	
		from.x = to.x;
		from.y = to.y;
		from.z = to.z + (2 * maxDimension);		

		Q3Point3D_Subtract(&to, &from, &viewVector);
		viewDistance = Q3Vector3D_Length(&viewVector);
		Q3Vector3D_Normalize(&viewVector, &normViewVector);
		
		maxDimension /= 2.0F;
		
		Q3Vector3D_Scale(&normViewVector, 
						viewDistance - maxDimension,
						&eyeToFrontClip);
						
		Q3Vector3D_Scale(&normViewVector, 
						viewDistance + maxDimension,
						&eyeToBackClip);
		
		hither 	= Q3Vector3D_Length(&eyeToFrontClip);
		yon 	= Q3Vector3D_Length(&eyeToBackClip);
		
		fieldOfView = Q3Math_RadiansToDegrees(1.25 * ErMath_Atan(maxDimension/hither));
	}

	{
		TQ3ViewAngleAspectCameraData 	data;
		TQ3Vector3D 	up 	= { 0.0F, 1.0F, 0.0F };

		data.cameraData.placement.cameraLocation 	= from;
		data.cameraData.placement.pointOfInterest 	= to;
		data.cameraData.placement.upVector 			= up;
	
		data.cameraData.range.hither = hither;
		data.cameraData.range.yon 	 = yon;
	
		data.cameraData.viewPort.origin.x = -1.0F;
		data.cameraData.viewPort.origin.y =  1.0F;
		data.cameraData.viewPort.width  = 2.0F;
		data.cameraData.viewPort.height = 2.0F;

		data.fov = Q3Math_DegreesToRadians(fieldOfView);
		
		{
			float w = (float)(theDocument->fWidth);
			float h = (float)(theDocument->fHeight);

			data.aspectRatioXToY = w/h;
		}
		
		if (theDocument->fView)
		{
			TQ3CameraObject camera;
			
			Q3View_GetCamera(theDocument->fView, &camera);
			if (camera) {
				Q3ViewAngleAspectCamera_SetData(camera, &data);
				Q3Object_Dispose(camera);
			}
			else {
				camera  = Q3ViewAngleAspectCamera_New (&data);
				if (camera) {
					Q3View_SetCamera (theDocument->fView, camera);
					Q3Object_Dispose(camera);
				}
			}
		}
	}
}

void pvBBox_Get(DocumentPtr theDocument, TQ3BoundingBox *bbox)
{			
	if (theDocument->fView)
	{
		Q3View_StartBoundingBox(theDocument->fView, kQ3ComputeBoundsExact);
		do
		{
			if (Q3DisplayGroup_Submit(theDocument->fModel, theDocument->fView) == kQ3Failure)
			{
				Q3View_Cancel(theDocument->fView);
				return;
			}
		} while (Q3View_EndBoundingBox(theDocument->fView, bbox) == kQ3ViewStatusRetraverse);
	}
	else
	{
		Q3Point3D_Set(&(bbox->min), -0.1F, -0.1F, -0.1F);
		Q3Point3D_Set(&(bbox->max), 0.1F, 0.1F, 0.1F);
		bbox->isEmpty = kQ3False;
	}
}

void pvBBoxCenter(TQ3BoundingBox *bbox, TQ3Point3D *center)
{
 	float	xSize, ySize, zSize;

	xSize = bbox->max.x - bbox->min.x;
	ySize = bbox->max.y - bbox->min.y;
	zSize = bbox->max.z - bbox->min.z;

	if (xSize <= kEPSILON &&
	    ySize <= kEPSILON &&
		zSize <= kEPSILON)  {
		bbox->max.x += 0.0001F;
		bbox->max.y += 0.0001F;
		bbox->max.z += 0.0001F;
		
		bbox->min.x -= 0.0001F;
		bbox->min.y -= 0.0001F;
		bbox->min.z -= 0.0001F;
	}
	
	center->x = (bbox->min.x + bbox->max.x) / 2.0F;
	center->y = (bbox->min.y + bbox->max.y) / 2.0F;
	center->z = (bbox->min.z + bbox->max.z) / 2.0F;
}
