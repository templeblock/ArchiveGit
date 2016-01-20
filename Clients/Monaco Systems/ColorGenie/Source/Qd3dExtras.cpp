#include "stdafx.h"
#include "Qd3dAll.h"
#include "Qd3dExtras.h"

/////////////////////////////////////////////////////////////////////////////
TQ3GroupObject OpenModel( char* szFileName )
{
	// Open the file and create QD3D storage object
	// Use Win32 Handle File Types 
	HANDLE hFile = CreateFile( szFileName,	// pointer to name of the file 
		GENERIC_READ,			// access (read-write) mode 
		0,						// share mode 
		NULL,					// pointer to security descriptor 
		OPEN_EXISTING,			// how to create 
		FILE_ATTRIBUTE_NORMAL,	// file attributes 
		NULL 					// handle to file with attributes to copy  
	);

	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;

	TQ3StorageObject aStorage = Q3Win32Storage_New(hFile);
	if (!aStorage)
		return NULL;

	TQ3FileObject aFileObject = Q3File_New();
	if (!aFileObject)
		return NULL;

	TQ3Status aStatus;
	aStatus = Q3File_SetStorage(aFileObject, aStorage);
	if (aStatus != kQ3Success)
		return NULL;

	TQ3FileMode aFileMode;
	aStatus = Q3File_OpenRead(aFileObject, &aFileMode);
	if (aStatus != kQ3Success)
		return NULL;

	TQ3GroupObject aGroup = Q3DisplayGroup_New();
	if (!aGroup)
		return NULL;

	TQ3Object anObject = NULL;
	while (Q3File_IsEndOfFile(aFileObject) == kQ3False)
	{
		anObject = Q3File_ReadObject(aFileObject);
		if (!anObject)
			return NULL;
		if (Q3Error_Get(NULL) != kQ3ErrorNone)
			return NULL;
		if (Q3Object_IsDrawable(anObject) == kQ3True)
		{
			TQ3GroupPosition aPosition = Q3Group_AddObject(aGroup, anObject);
			if (!aPosition)
				return NULL;
		}

		if (Q3Object_Dispose(anObject) != kQ3Success)
			return NULL;
		anObject = NULL;
	}

	aStatus = Q3File_Close(aFileObject);
	if (aStatus != kQ3Success)
		return NULL;
	
	CloseHandle( hFile );

	aStatus = Q3Object_Dispose(aFileObject);
	if (aStatus != kQ3Success)
		return NULL;
	aStatus = Q3Object_Dispose(aStorage);
	if (aStatus != kQ3Success)
		return NULL;

	return aGroup;
}

/////////////////////////////////////////////////////////////////////////////
void SaveModel( char* szFileName, TQ3GroupObject aModel )
{
	HANDLE hFile = CreateFile( szFileName,	// pointer to name of the file 
		GENERIC_WRITE,			// access (read-write) mode 
		0,						// share mode 
		NULL,					// pointer to security descriptor 
		CREATE_ALWAYS ,			// how to create 
		FILE_ATTRIBUTE_NORMAL,	// file attributes 
		NULL 					// handle to file with attributes to copy  
	);

	if (hFile == INVALID_HANDLE_VALUE)
		return;

	TQ3StorageObject aStorage = Q3Win32Storage_New(hFile);
	if (!aStorage)
		return;

	TQ3FileObject aFileObject = Q3File_New();
	if (!aFileObject)
		return;

	TQ3Status aStatus;
	aStatus = Q3File_SetStorage(aFileObject, aStorage);
	if (aStatus != kQ3Success)
		return;

	TQ3ViewObject aView = Q3View_New();
	if (!aView)
		return;

	TQ3FileMode aFileMode = kQ3FileModeNormal;
	aStatus = Q3File_OpenWrite(aFileObject, aFileMode);
	if (aStatus != kQ3Success)
		return;

	aStatus = Q3View_StartWriting(aView, aFileObject);
	if (aStatus != kQ3Success)
		return;

	TQ3ViewStatus aViewStatus;
	do
	{
		aStatus = Q3Object_Submit(aModel, aView);
		if (aStatus != kQ3Success)
			return;
		aViewStatus = Q3View_EndWriting(aView);
		if (aViewStatus == kQ3ViewStatusError)
			return;
	} while (aViewStatus == kQ3ViewStatusRetraverse);

	aStatus = Q3File_Close(aFileObject);
	if (aStatus != kQ3Success)
		return;

	CloseHandle( hFile );
}

/////////////////////////////////////////////////////////////////////////////
TQ3DrawContextObject MyNewDrawContext(HWND hWnd, LPRECT pRect)
{
	// set the pane
	TQ3Area pane;
	pane.min.x = (float)pRect->left;
	pane.min.y = (float)pRect->top;
	pane.max.x = (float)pRect->right;
	pane.max.y = (float)pRect->bottom;
	
	//	Set the background color.
	TQ3ColorARGB ClearColor;
	ClearColor.a = 1.0;
	ClearColor.r = 0.0;
	ClearColor.g = 0.0;
	ClearColor.b = 0.0;
	
	//	Fill in draw context data.
	TQ3DrawContextData myDrawContextData;
	myDrawContextData.clearImageMethod = kQ3ClearMethodWithColor;
	myDrawContextData.clearImageColor = ClearColor;
	myDrawContextData.paneState = kQ3True; //was kQ3True;
	myDrawContextData.pane = pane;
	myDrawContextData.maskState = kQ3False;
	myDrawContextData.doubleBufferState = kQ3True;
 
	TQ3Win32DCDrawContextData myWinDrawContextData;
	myWinDrawContextData.drawContextData = myDrawContextData;
	myWinDrawContextData.hdc = ::GetDC(hWnd);
	
	//	Create draw context and return it, if it's NULL the caller must handle
	return Q3Win32DCDrawContext_New(&myWinDrawContextData);
}

/////////////////////////////////////////////////////////////////////////////
TQ3CameraObject MyNewCamera(LPRECT pRect, TQ3Point3D *cameraFrom, TQ3Point3D *cameraTo)
{
	TQ3Vector3D cameraUp = { 0.0, 1.0, 0.0 };
	double fieldOfView = .52359333333;
	double hither = 0.001;
	double yon = 1000;

	//	Fill in camera data.
	TQ3CameraData myCameraData;
	myCameraData.placement.cameraLocation = *cameraFrom;
	myCameraData.placement.pointOfInterest = *cameraTo;
	myCameraData.placement.upVector = cameraUp;
	
	myCameraData.range.hither = (float)hither;
	myCameraData.range.yon = (float)yon;
	
	TQ3ViewAngleAspectCameraData myViewAngleCameraData;
	myCameraData.viewPort.origin.x = -1.0;
	myCameraData.viewPort.origin.y = 1.0;
	myCameraData.viewPort.width = 2.0;
	myCameraData.viewPort.height = 2.0;
	
	myViewAngleCameraData.cameraData = myCameraData;
	myViewAngleCameraData.fov = (float)fieldOfView;
	
	// set up the aspect ratio based on the window
	myViewAngleCameraData.aspectRatioXToY =  
			(float) (pRect->right - pRect->left) / 
			(float) (pRect->bottom - pRect->top);

	TQ3CameraObject myCamera = Q3ViewAngleAspectCamera_New(&myViewAngleCameraData);	
	
	//	Return the camera.
	return ( myCamera );
}

/////////////////////////////////////////////////////////////////////////////
TQ3GroupObject MyNewLights(double ambient, double point, double fill)
{
	//	Set up light data for ambient light.  This light data will be used for point and fill
	//	light also.

	TQ3ColorRGB WhiteLight = { 1.0, 1.0, 1.0 };
	TQ3LightData myLightData;
	myLightData.isOn = kQ3True;
	myLightData.color = WhiteLight;
	
	//	Create ambient light.
	myLightData.brightness = (float)ambient;

	TQ3LightObject myAmbientLight;
	myAmbientLight = Q3AmbientLight_New(&myLightData);
	if ( !myAmbientLight )
		goto bail;
	
	//	Create point light.
	myLightData.brightness = (float)point;

	TQ3PointLightData myPointLightData;
	myPointLightData.lightData = myLightData;
	myPointLightData.castsShadows = kQ3False;
	myPointLightData.attenuation = kQ3AttenuationTypeNone;
	{
	TQ3Point3D pointLocation = { -10.0, 0.0, 10.0 };
	myPointLightData.location = pointLocation;
	}
	TQ3LightObject myPointLight;
	myPointLight = Q3PointLight_New(&myPointLightData);
	if ( !myPointLight )
		goto bail;

	//	Create fill light.
	myLightData.brightness = (float)fill;

	TQ3DirectionalLightData myDirectionalLightData;
	myDirectionalLightData.lightData = myLightData;
	myDirectionalLightData.castsShadows = kQ3False;
	{
	TQ3Vector3D fillDirection = { 10.0, 0.0, 10.0 };
	myDirectionalLightData.direction = fillDirection;
	}
	TQ3LightObject myFillLight;
	myFillLight = Q3DirectionalLight_New(&myDirectionalLightData);
	if ( !myFillLight )
		goto bail;

	//	Create light group and add each of the lights into the group.
	TQ3GroupObject myLightList;
	myLightList = Q3LightGroup_New();
	if ( !myLightList )
		goto bail;

	TQ3GroupPosition myGroupPosition;
	myGroupPosition = Q3Group_AddObject(myLightList, myAmbientLight);
	if ( myGroupPosition == 0 )
		goto bail;
	myGroupPosition = Q3Group_AddObject(myLightList, myPointLight);
	if ( myGroupPosition == 0 )
		goto bail;
	myGroupPosition = Q3Group_AddObject(myLightList, myFillLight);
	if ( myGroupPosition == 0 )
		goto bail;

	Q3Object_Dispose( myAmbientLight );
	Q3Object_Dispose( myPointLight );
	Q3Object_Dispose( myFillLight );

	//	Done!
	return ( myLightList );
	
bail:
	//	If any of the above failed, then return nothing!
	return ( NULL );
}

/////////////////////////////////////////////////////////////////////////////
TQ3ViewObject MyNewView(HWND hWnd, LPRECT pRect, double ambient, double point, double fill,TQ3Point3D *from, TQ3Point3D *to)
{
	TQ3ViewObject myView;
	if (!(myView = Q3View_New()))
		goto bail;	
		
	//	Create and set draw context.
	TQ3DrawContextObject myDrawContext;
	if (!(myDrawContext = MyNewDrawContext(hWnd, pRect)))
		goto bail;
		
	TQ3Status myStatus;
	if ((myStatus = Q3View_SetDrawContext(myView, myDrawContext)) == kQ3Failure )
		goto bail;

	Q3Object_Dispose( myDrawContext );
	
	//	Create and set renderer.
	// this would use the interactive software renderer
	BOOL bWireFrame;
	bWireFrame = FALSE;
	unsigned long RenderType;
	RenderType = (bWireFrame ? kQ3RendererTypeWireFrame : kQ3RendererTypeInteractive);

	TQ3RendererObject myRenderer;
	if (myRenderer = Q3Renderer_NewFromType(RenderType))
	{
		if ((myStatus = Q3View_SetRenderer(myView, myRenderer)) == kQ3Failure )
			goto bail;
		
		if (RenderType == kQ3RendererTypeInteractive)
		{
			// these two lines set us up to use the best possible renderer,
			// including  hardware if it is installed.
			Q3InteractiveRenderer_SetDoubleBufferBypass(myRenderer, kQ3True);						
			Q3InteractiveRenderer_SetPreferences(myRenderer, kQAVendor_BestChoice, 0);
		}
	}
	else
		goto bail;

	Q3Object_Dispose( myRenderer );
	
	//	Create and set camera.
	TQ3CameraObject myCamera;
	if (!(myCamera = MyNewCamera(pRect,from,to)))
		goto bail;
		
	if ((myStatus = Q3View_SetCamera(myView, myCamera)) == kQ3Failure )
		goto bail;

	Q3Object_Dispose( myCamera );
	
	//	Create and set lights.
	TQ3GroupObject myLights;
	if (!(myLights = MyNewLights(ambient, point, fill)))
		goto bail;
		
	if ((myStatus = Q3View_SetLightGroup(myView, myLights)) == kQ3Failure )
		goto bail;
		
	Q3Object_Dispose(myLights);

	return ( myView );
	
bail:
	//	If any of the above failed, then don't return a view.
	return ( NULL );
}

/////////////////////////////////////////////////////////////////////////////
TQ3GroupObject MyNewModel(void)
{
	// Create a group for the complete model.
	// do not use Q3OrderedDisplayGroup_New since in this
	// type of group all of the translations are applied before
	// the objects in the group are drawn, in this instance we 
	// dont want this.
	TQ3ShaderObject myIlluminationShader;
	TQ3GroupObject myGroup;
	if ((myGroup = Q3DisplayGroup_New()) != NULL )
	{
		// Define a shading type for the group
		// and add the shader to the group
		myIlluminationShader = Q3PhongIllumination_New();
		//myIlluminationShader = Q3NULLIllumination_New();
		//myIlluminationShader = Q3LambertIllumination_New();
		Q3Group_AddObject(myGroup, myIlluminationShader);
	}
	
	// dispose of the objects we created here
	if ( myIlluminationShader ) 
		Q3Object_Dispose(myIlluminationShader);	

	//	Done!
	return ( myGroup );
}

#define REFERROR 00L
#define REFWARNING 01L
#define REFNOTICE 02L

/////////////////////////////////////////////////////////////////////////////
static void QD3D_CALLBACK ErrorHandler(TQ3Error firstError, TQ3Error lastError, long refCon) 
{ 
	char* pType;
	if (refCon == REFERROR)   pType = "Error";   else
	if (refCon == REFWARNING) pType = "Warning"; else
	if (refCon == REFNOTICE)  pType = "Notice";	 else return;

	char szBuffer[512];
	sprintf(szBuffer, "QD3D %s %d\n", pType, lastError); 
	OutputDebugString(szBuffer); 
}

/////////////////////////////////////////////////////////////////////////////
void SetQd3dErrorHandler(void)
{
	TQ3ErrorMethod error = ErrorHandler;
	Q3Error_Register( (TQ3ErrorMethod)error, REFERROR );
	Q3Warning_Register( (TQ3WarningMethod)error, REFWARNING );
	Q3Notice_Register( (TQ3NoticeMethod)error, REFNOTICE );
}
