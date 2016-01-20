#include "stdafx.h"
#include "Qd3dAll.h"
#include "Qd3dGamutBase.h"
#include "Qd3dExtras.h"
#include "xyztorgb.h"
#include "MathUtilities.h"
#include "colortran.h"
//#include "BuildProgress.h"

/////////////////////////////////////////////////////////////////////////////
CQd3dGamutBase::CQd3dGamutBase()
{
	if (Q3Initialize() == kQ3Failure)
		;
	Document.fModel = NULL;
	_hGamutData = NULL;
	_monitor_z = 1.8;
	memset(&_box_rect, 0, sizeof(_box_rect));
	memset(&Document, 0, sizeof(Document));
	_ambient = 0.6;
	_point = 0.8;
	_fill = 0.2;
	_hWndGamut = NULL;
	_deleteflag = FALSE;

	Monitor monitor;
	monitor.setup[0] = .0000; // Gamma (not read by newbuildrgbxyz())
	monitor.setup[1] = .3126; // WhiteX
	monitor.setup[2] = .3289; // WhiteY
	monitor.setup[3] = .6584; // RedX
	monitor.setup[4] = .3377; // RedY
	monitor.setup[5] = .2799; // GreenX
	monitor.setup[6] = .7200; // GreenY
	monitor.setup[7] = .1342; // BlueX
	monitor.setup[8] = .0720; // BlueY
	_XyztoRgb.xyz2rgb(&monitor);
}

/////////////////////////////////////////////////////////////////////////////
CQd3dGamutBase::~CQd3dGamutBase()
{	
	if(_hGamutData && _deleteflag)
	{
		McoDeleteHandle(_hGamutData);
		_hGamutData = 0;
	}		

	DisposeDocumentData();

	// Close our connection to the QuickDraw 3D library
	TQ3Status myStatus = Q3Exit();
}		
		
/////////////////////////////////////////////////////////////////////////////
McoStatus CQd3dGamutBase::InitDocumentData() 
{
	// sets up the 3d data for the scene
	Document.fCameraFrom.x = 0.0; 
	Document.fCameraFrom.y = 0.0;
	Document.fCameraFrom.z = 30.0;
	
	Document.fCameraTo.x = 0.0; 
	Document.fCameraTo.y = 0.0;
	Document.fCameraTo.z = 0.0;
	
	//	Create view for QuickDraw 3D.
	Document.fView = MyNewView( _hWndGamut, &_box_rect, _ambient, _point, _fill, &Document.fCameraFrom, &Document.fCameraTo);

	// the main display group:
	Document.fModel = MyNewModel();
	
	// scale and group center
	TQ3Point3D myOrigin = { 0, 0, 0 };
	Document.fGroupScale = 1;				
	Document.fGroupCenter = myOrigin;
	
	//j pvCamera_Fit(theDocument);

	// the drawing styles:
	Document.fInterpolation = Q3InterpolationStyle_New(kQ3InterpolationStyleVertex);
	Document.fBackFacing = Q3BackfacingStyle_New(kQ3BackfacingStyleBoth);
	Document.fFillStyle = Q3FillStyle_New(kQ3FillStyleFilled);
	Document.fIllumination = Q3PhongIllumination_New();

	// set the rotation matrix the identity matrix
	Q3Matrix4x4_SetIdentity(&Document.fRotation);	
					
	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
void CQd3dGamutBase::DisposeDocumentData()
{
	if (Document.fView)
		Q3Object_Dispose(Document.fView);			// the view for the scene
	if (Document.fModel)
		Q3Object_Dispose(Document.fModel);			// object in the scene being modelled
	if (Document.fInterpolation)
		Q3Object_Dispose(Document.fInterpolation);	// interpolation style used when rendering
	if (Document.fBackFacing)
		Q3Object_Dispose(Document.fBackFacing);		// whether to draw shapes that face away from the camera
	if (Document.fFillStyle)
		Q3Object_Dispose(Document.fFillStyle);		// whether drawn as solid filled object or decomposed to components
	if (Document.fIllumination)
	 	Q3Object_Dispose(Document.fIllumination);
}

/////////////////////////////////////////////////////////////////////////////
// initialize the 3D and load an object
McoStatus CQd3dGamutBase::Init(char* szFileName, HWND hWnd)
{
	SetQd3dErrorHandler();

	if (!(_hGamutData = ReadGamutDataFile(szFileName)))
		return MCO_FAILURE;

	_hWndGamut = hWnd;
	::GetClientRect(_hWndGamut, &_box_rect);

	// initialize our document structure
	InitDocumentData();
	
	int iFOVMultiplier = CreateGeometry();
	AdjustCamera((short)(_box_rect.right - _box_rect.left), (short)(_box_rect.bottom - _box_rect.top), iFOVMultiplier);

	// Uncomment this code if you want to save the model to a file for viewing later
	//SaveModel( "c:\\Windows\\Desktop\\Gamut.3dmf", Document.fModel );

	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// initialize the 3D and load an object
//P
//McoStatus CQd3dGamutBase::InitFromDoc(RawData* rawdata, HWND hWnd)
McoStatus CQd3dGamutBase::InitFromDoc(McoHandle gamut, HWND hWnd)
{
	McoStatus status;

	SetQd3dErrorHandler();

	_hGamutData = gamut;

	_hWndGamut = hWnd;
	::GetClientRect(_hWndGamut, &_box_rect);

	// initialize our document structure
	InitDocumentData();
	
	int iFOVMultiplier = CreateGeometry();
	AdjustCamera((short)(_box_rect.right - _box_rect.left), (short)(_box_rect.bottom - _box_rect.top), iFOVMultiplier);

	// Uncomment this code if you want to save the model to a file for viewing later
	//SaveModel( "c:\\Windows\\Desktop\\Gamut.3dmf", Document.fModel );

	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// THIS VIRTUAL FUNCTION SHOW MUST BE OVERRIDDEN TO CREATE THE OBJECTS
int /*iFOVMultiplier*/ CQd3dGamutBase::CreateGeometry(void)
{
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
McoHandle CQd3dGamutBase::ReadGamutDataFile(char* szFileName)
{
	if (!szFileName)
		return NULL;

	// raw will be deleted by _GamutData, once he takes ownership
	RawData raw;

	FILE* fs = fopen( szFileName, "r" );
	if (!fs)
	{
		return NULL;
	}

	raw.loadData(fs);
	fclose(fs);

	const char*	name = strrchr( szFileName, '\\' );
	if( name != NULL ) name++;

	McoHandle	gamut;
//j	CBuildProgress	buildprogress( (long)&raw, (long)&gamut, Process_GammutSurface, name);
//j	if(buildprogress.DoModal() == IDCANCEL)
//j	{
//j		return NULL;
//j	}


/*
	McoStatus status;
	status = _GamutData.Init(raw, 0);
	delete raw;
	if (status != MCO_SUCCESS)
	{
		return NULL;
	}
*/

	// raw will now be deleted by _GamutData; he took ownership
	return gamut;
}

/////////////////////////////////////////////////////////////////////////////
// set the viewing area so that the object is never clipped
TQ3Point3D CQd3dGamutBase::AdjustCamera(short winWidth, short winHeight, int iFOVMultiplier)
{
	TQ3ViewObject theView = Document.fView;
	TQ3GroupObject mainGroup = Document.fModel;
	TQ3Point3D *documentGroupCenter = &Document.fGroupCenter;
	double *documentGroupScale  = &Document.fGroupScale;

	TQ3CameraObject camera;
	Q3View_GetCamera(theView, &camera);
	TQ3BoundingBox viewBBox;
	GetGroupBBox(&viewBBox);

	// If we have a point model, then the "viewBBox" would end up
	// being a "singularity" at the location of the point.  As
	// this bounding "box" is used in setting up the camera spec,
	// we get bogus input into Escher.
	float xSize = viewBBox.max.x - viewBBox.min.x;
	float ySize = viewBBox.max.y - viewBBox.min.y;
	float zSize = viewBBox.max.z - viewBBox.min.z;

	if (xSize <= kQ3RealZero && ySize <= kQ3RealZero && zSize <= kQ3RealZero)
	{
		viewBBox.max.x += (float)0.0001;
		viewBBox.max.y += (float)0.0001;
		viewBBox.max.z += (float)0.0001;
		
		viewBBox.min.x -= (float)0.0001;
		viewBBox.min.y -= (float)0.0001;
		viewBBox.min.z -= (float)0.0001;
	}

	TQ3Point3D points[2];
	points[0] = viewBBox.min;
	points[1] = viewBBox.max;

	float weights[2] = { 0.5, 0.5 };
	Q3Point3D_AffineComb(points, weights, 2, documentGroupCenter);

	// The "from" point is on a vector perpendicular to the plane
	// in which the bounding box has greatest dimension.  As "up" is
	// always in the positive y direction, look at x and z directions.
	xSize = viewBBox.max.x - viewBBox.min.x;
	zSize = viewBBox.max.z - viewBBox.min.z;
	
	long fromAxis;	
	if (xSize > zSize)
		fromAxis = kQ3AxisZ;
	else
		fromAxis = kQ3AxisX;

	// Compute the length of the diagonal of the bounding box.
	// 
	// The hither and yon planes are adjusted so that the
 	// diagonal of the bounding box is 7/8 the size of the
 	// minimum dimension of the view frustum. The diagonal is used instead
 	// of the maximum size (in x, y, or z) so that when you rotate
 	// the object, the corners don't get clipped out.
	TQ3Vector3D diagonalVector;
	Q3Point3D_Subtract(&viewBBox.max, &viewBBox.min, &diagonalVector);

	float maxDimension;
	maxDimension  = Q3Vector3D_Length(&diagonalVector);
	maxDimension *= (float)(8.0 / 7.0);
	
	float ratio = (float)1.0 / maxDimension;
	*documentGroupScale = ratio;
	
	TQ3CameraPlacement placement;
	Q3Camera_GetPlacement(camera, &placement);

	TQ3Vector3D viewVector;
	Q3Point3D_Subtract(
		&placement.cameraLocation,
		&placement.pointOfInterest,
		&viewVector);
		
	float viewDistance = Q3Vector3D_Length(&viewVector);
	
	TQ3Vector3D normViewVector;
	Q3Vector3D_Normalize(&viewVector, &normViewVector);
	
	TQ3Vector3D eyeToFrontClip;
	Q3Vector3D_Scale(&normViewVector, 
					 (float)(viewDistance - ratio * maxDimension/2.0),
					 &eyeToFrontClip);
					
	TQ3Vector3D eyeToBackClip;
	Q3Vector3D_Scale(&normViewVector, 
					(float)(viewDistance + ratio * maxDimension/2.0),
					&eyeToBackClip);

	float hither = Q3Vector3D_Length(&eyeToFrontClip);
	float yon = Q3Vector3D_Length(&eyeToBackClip);
	
	float fieldOfView = (float)(iFOVMultiplier * atan((ratio * maxDimension/2.0)/hither));

	TQ3CameraRange range;
	range.hither = hither;
	range.yon = yon;

	Q3Camera_SetRange(camera, &range);

	Q3ViewAngleAspectCamera_SetFOV(camera, fieldOfView);

	Q3ViewAngleAspectCamera_SetAspectRatio(camera, (float) winWidth / (float) winHeight);

	Q3Object_Dispose(camera);
	
	return( *documentGroupCenter );
}

/////////////////////////////////////////////////////////////////////////////
void CQd3dGamutBase::GetGroupBBox( TQ3BoundingBox* viewBBox )
{
#ifdef BETA_1_BUILD
	Q3View_StartBounds( Document.fView );
	TQ3GroupObject mainGroup = Document.fModel;
	TQ3Status status = Q3DisplayGroup_BoundingBox(mainGroup, viewBBox,
		kQ3ComputeBoundsApproximate, viewObject);
	Q3View_EndBounds( Document.fView );
#else
	TQ3ViewStatus viewStatus;
	TQ3Status status = Q3View_StartBoundingBox( Document.fView, kQ3ComputeBoundsApproximate );
	do {
		status = SubmitScene();
	} while ((viewStatus = Q3View_EndBoundingBox( Document.fView, viewBBox )) == kQ3ViewStatusRetraverse );
#endif
						        
	//  If we have a point model, then the "viewBBox" would end up
	//  being a "singularity" at the location of the point.  As
	//  this bounding "box" is used in setting up the camera spec,
	//  we get bogus input into Escher.
	float xSize = viewBBox->max.x - viewBBox->min.x;
	float ySize = viewBBox->max.y - viewBBox->min.y;
	float zSize = viewBBox->max.z - viewBBox->min.z;

	if (xSize <= kQ3RealZero && ySize <= kQ3RealZero && zSize <= kQ3RealZero)
	{
		viewBBox->max.x += (float)0.0001;
		viewBBox->max.y += (float)0.0001;
		viewBBox->max.z += (float)0.0001;
		
		viewBBox->min.x -= (float)0.0001;
		viewBBox->min.y -= (float)0.0001;
		viewBBox->min.z -= (float)0.0001;
	}
}

/////////////////////////////////////////////////////////////////////////////
// assumes the port is set up before being called
TQ3Status CQd3dGamutBase::DocumentDraw3DData()
{	
	//	Start rendering.
	TQ3Status status;	
	status = Q3View_StartRendering(Document.fView);
	do {
		status = SubmitScene();
	} while (Q3View_EndRendering(Document.fView) == kQ3ViewStatusRetraverse );

	return status;
}

/////////////////////////////////////////////////////////////////////////////
// Submit the scene for rendering/fileIO and picking
TQ3Status CQd3dGamutBase::SubmitScene(void) 
{		
	TQ3Vector3D globalScale;
	globalScale.x = globalScale.y = globalScale.z = (float)Document.fGroupScale;

	TQ3Vector3D globalTranslate;
	globalTranslate = *(TQ3Vector3D *)&Document.fGroupCenter;

	Q3Vector3D_Scale(&globalTranslate, -1, &globalTranslate);
	Q3Shader_Submit(Document.fIllumination, Document.fView);
	Q3Style_Submit(Document.fInterpolation, Document.fView);
	Q3Style_Submit(Document.fBackFacing , Document.fView);
	Q3Style_Submit(Document.fFillStyle, Document.fView);
	Q3MatrixTransform_Submit(&Document.fRotation, Document.fView);
	Q3ScaleTransform_Submit(&globalScale, Document.fView);
	Q3TranslateTransform_Submit(&globalTranslate, Document.fView);
		
	return Q3DisplayGroup_Submit(Document.fModel, Document.fView);
}

/////////////////////////////////////////////////////////////////////////////
void CQd3dGamutBase::DoTransform(int code, double v1, double v2, double v3)
{
	if (code == WE_Rotate)
	{
		TQ3Matrix4x4 tmp;
		Q3Matrix4x4_SetRotate_XYZ(&tmp, (float)v1, (float)v2, (float)v3);
		Q3Matrix4x4_Multiply(&Document.fRotation, &tmp, &Document.fRotation);
	}
	else
	if (code == WE_Scale)
	{
		Document.fGroupScale += v1;
		if (Document.fGroupScale < 0)
			Document.fGroupScale = 0;
	}
	else
	if (code == WE_Translate)
	{
		double xt = -Document.fRotation.value[0][0]*v1+Document.fRotation.value[1][0]*v2;
		double yt = -Document.fRotation.value[0][1]*v1+Document.fRotation.value[1][1]*v2;
		double zt = Document.fRotation.value[0][2]*v1-Document.fRotation.value[1][2]*v2;
		Document.fGroupCenter.x += (float)xt;
		Document.fGroupCenter.y += (float)yt;
		Document.fGroupCenter.z += (float)zt;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CQd3dGamutBase::GetRotation(TQ3Matrix4x4 *rotation)
{
	Q3Matrix4x4_Copy(&Document.fRotation, rotation);
}

/////////////////////////////////////////////////////////////////////////////
void CQd3dGamutBase::SetRotation(TQ3Matrix4x4 *rotation)
{
	Q3Matrix4x4_Copy(rotation, &Document.fRotation);
}


/////////////////////////////////////////////////////////////////////////////
// given a local to global array, convert a global point to local
// usefull for picking when local coordinates of a picked object are needed
void CQd3dGamutBase::TranslateGlobalToLocal(TQ3Matrix4x4* LtoG, TQ3Point3D* GPoint, TQ3Point3D* LPoint)
{
	TQ3Matrix4x4 GtoL;
	Q3Matrix4x4_Copy(LtoG, &GtoL);
	Q3Matrix4x4_Invert(&GtoL, &GtoL);
	Q3Point3D_Transform(GPoint, &GtoL, LPoint);
}

/////////////////////////////////////////////////////////////////////////////
McoStatus CQd3dGamutBase::Get3Dinfo(POINT Point, double *lab, int gamutCheck, BOOL* ingamut)
{
	// Get the window coordinates of a mouse click.
	TQ3Point2D my2DPoint;
	my2DPoint.x = Point.x;
	my2DPoint.y = Point.y;

	// Set up picking data structures.
	// Set sorting type: objects nearer to pick origin are returned first.
	TQ3WindowPointPickData myWPPickData;
	myWPPickData.data.sort = kQ3PickSortNearToFar; //kQ3PickSortNone
	myWPPickData.data.mask = kQ3PickDetailMaskPickID | kQ3PickDetailMaskXYZ | 
			 				 kQ3PickDetailMaskObject | kQ3PickDetailMaskLocalToWorldMatrix;
	myWPPickData.data.numHitsToReturn = 1; // kQ3ReturnAllHits;
	myWPPickData.point = my2DPoint;
	myWPPickData.vertexTolerance = 1.0; // 2.0
	myWPPickData.edgeTolerance = 1.0; // 2.0

	// Create a new window-point pick object.
	TQ3PickObject myPickObject = Q3WindowPointPick_New(&myWPPickData);

	// Pick a group object.
	TQ3ViewStatus viewStatus;
	TQ3Status status = Q3View_StartPicking(Document.fView, myPickObject);
	do {
		status = SubmitScene(); // Q3DisplayGroup_Submit(Document.fModel, Document.fView);
	} while ((viewStatus = Q3View_EndPicking(Document.fView)) == kQ3ViewStatusRetraverse);

	// See whether any hits occurred.
	unsigned long myNumHits;
	long state = Q3Pick_GetNumHits(myPickObject, &myNumHits);
	if ((state != kQ3Success) || (myNumHits == 0))
	{
		Q3Object_Dispose(myPickObject);
		return MCO_FAILURE;
	}

	// Process each hit.
	TQ3Object object = NULL;
	for (unsigned long myIndex = 0; myIndex < myNumHits; myIndex++)
	{
		TQ3PickDetail validMask;
		Q3Pick_GetPickDetailValidMask(myPickObject, myIndex, &validMask);
		
		if (!(validMask & kQ3PickDetailMaskObject)  ||
		    !(validMask & kQ3PickDetailMaskXYZ)		||
		    !(validMask & kQ3PickDetailMaskLocalToWorldMatrix))
				return MCO_FAILURE;
	
		Q3Pick_GetPickDetailData(myPickObject, myIndex, kQ3PickDetailMaskObject, &object);
		// operate on myHitData, then...
		
		*ingamut = (Q3Geometry_GetType(object) == kQ3GeometryTypeMesh);
//		BOOL get_point = (*ingamut == gamutCheck);
//		if(gamutCheck == 0)	get_point = TRUE;
//		if (get_point)
//		{	
			TQ3Point3D point;
			TQ3Point3D LPoint;
			TQ3Matrix4x4 LtoG;
			Q3Pick_GetPickDetailData(myPickObject, myIndex, kQ3PickDetailMaskLocalToWorldMatrix, &LtoG);
			Q3Pick_GetPickDetailData(myPickObject, myIndex, kQ3PickDetailMaskXYZ, &point);
			TranslateGlobalToLocal(&LtoG,&point,&LPoint);
			// lab[0] should be set by the caller
			lab[1] = 128*LPoint.x;
			lab[2] = 128*LPoint.y;
//		}
//		else
//		{ // return "nothing"
//			lab[1] = 0.0;
//			lab[2] = 0.0;
//		}

		if (object)
			Q3Object_Dispose(object);
	}

	// Dispose of all hits in the hit list.
	Q3Pick_EmptyHitList(myPickObject);

	// Dispose of the pick object.
	Q3Object_Dispose(myPickObject);
	return MCO_SUCCESS;
}
