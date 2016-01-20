#include "stdafx.h"
#include "Qd3dGamutTweak.h"
#include "mathutilities.h"
#include "colortran.h"

#define CIRCLE_STEP 9

const double klab1[3] = { 50, 0, 0 };
const double klab2[3] = { 50, 4, 0 };

/////////////////////////////////////////////////////////////////////////////
CQd3dGamutTweak::CQd3dGamutTweak()
{
	_pTweak = NULL;
	_SliderL = 50;
	_indexlut = NULL;
	_minL = 0;
	_maxL = 100;
}

/////////////////////////////////////////////////////////////////////////////
CQd3dGamutTweak::~CQd3dGamutTweak()
{
	if (_indexlut)
		delete _indexlut;
}

/////////////////////////////////////////////////////////////////////////////
int /*iFOVMultiplier*/ CQd3dGamutTweak::CreateGeometry(void)
{
	McoStatus status;

	status = ChangeGeometry(MP_TweakAx,0);
	if (status != MCO_SUCCESS)
		;
			
	status = ChangeGeometry(MP_Circle,0);
	if (status != MCO_SUCCESS)
		;
	
	status = ChangeGeometry(MP_Arrow,0);
	if (status != MCO_SUCCESS)
		;
	
	status = ChangeGeometry(MP_GamutSlice,0);
	if (status != MCO_SUCCESS)
		;
	
	status = ChangeGeometry(MP_BlackBox,0);
	if (status != MCO_SUCCESS)
		;
	
	status = ChangeGeometry(MP_TweakMark1,0);
	if (status != MCO_SUCCESS)
		;

	return 1; /*iFOVMultiplier*/
}

/////////////////////////////////////////////////////////////////////////////
McoStatus CQd3dGamutTweak::ChangeGeometry(Qd3dType type, long num)
{
	if (!Document.fModel)
		return MCO_OBJECT_NOT_INITIALIZED;

	TQ3Object object = NULL;
	TQ3GroupPosition position = 0L;
	TQ3GroupPosition last_position = 0L;
	TQ3Status status = Q3Group_GetFirstPositionOfType(Document.fModel, kQ3ShapeTypeGeometry, &position);
	while (status == kQ3Success && position)
	{
		last_position = position;
		Q3Group_GetNextPositionOfType(Document.fModel, kQ3ShapeTypeGeometry, &position);
	}

	switch (type)
	{
		case MP_GamutSlice:
			object = NewGamutObject(TRUE/*bDidGamutCompression*/);
			break;
		case MP_TweakAx:
			object = New_ab_ax();
			break;	
		case MP_Circle:
			object = NewCircle();
			break;
		case MP_Arrow:
			object = NewArrow();
			break;
		case MP_BlackBox:
			object = NewBlackBox();
			break;
		case MP_TweakMark1:
			object = MakeCross(TRUE/*bNew*/);
			break;
		case MP_TweakMark2:
		default:
			break;
	}
			
	if (!object)
		return MCO_QD3D_ERROR;
	if (last_position)
		Q3Group_AddObjectBefore(Document.fModel, last_position, object);
	else
		Q3Group_AddObject(Document.fModel, object);

	Q3Object_Dispose(object);
	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
static void InvalidateRoot(HWND hWnd)
{
	if (!hWnd)
		return;

	HWND hChild = ::GetTopWindow(hWnd);
	if (!hChild)
	{ // If no children
		::InvalidateRect(hWnd, NULL, FALSE);
		::UpdateWindow(hWnd);
		return;
	}

	// Otherwise, redraw all children instead
	while (hChild)
	{
		::InvalidateRect(hChild, NULL, FALSE);
		::UpdateWindow(hChild);
		hChild = ::GetNextWindow(hChild, GW_HWNDNEXT);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CQd3dGamutTweak::ChangeTweakData(Tweak_Element* pTweak, int iSliderL)
{
	_pTweak = pTweak;
	_SliderL = iSliderL;

	ModifyGamutObject();
	ModifyCircle();
	ModifyArrow();
	MakeCross(FALSE/*bNew*/);

	InvalidateRoot(_hWndGamut);
}

/////////////////////////////////////////////////////////////////////////////
// create a black box behind everything for selecting out of gamut colors
TQ3GeometryObject CQd3dGamutTweak::NewBlackBox(void)
{
	TQ3PolygonData	poly;
	poly.vertices = new TQ3Vertex3D[4];
	poly.numVertices  = 4;
	poly.polygonAttributeSet = 0L;

	poly.vertices[0].point.x = 1;
	poly.vertices[0].point.y = 1;
	poly.vertices[0].point.z = 0.49;
	poly.vertices[0].attributeSet = 0L;

	poly.vertices[1].point.x = -1;
	poly.vertices[1].point.y = 1;
	poly.vertices[1].point.z = 0.49;
	poly.vertices[1].attributeSet = 0L;

	poly.vertices[2].point.x = -1;
	poly.vertices[2].point.y = -1;
	poly.vertices[2].point.z = 0.49;
	poly.vertices[2].attributeSet = 0L;

	poly.vertices[3].point.x = 1;
	poly.vertices[3].point.y = -1;
	poly.vertices[3].point.z = 0.49;
	poly.vertices[3].attributeSet = 0L;

	poly.polygonAttributeSet = Q3AttributeSet_New();

	TQ3ColorRGB color  = { 0.0, 0.0, 0.0 };
	Q3AttributeSet_Add(poly.polygonAttributeSet, kQ3AttributeTypeDiffuseColor, &color);

	TQ3GeometryObject object = Q3Polygon_New(&poly);
	delete poly.vertices;
	return object;
}

/////////////////////////////////////////////////////////////////////////////
McoStatus CQd3dGamutTweak::ModifyGamutObject(void)
{
	// get the object position
	TQ3GroupPosition position;
	TQ3Status status = Q3Group_GetFirstPositionOfType(Document.fModel, kQ3GeometryTypeMesh, &position);
	if (!position)
		return MCO_FAILURE;

	// delete it 
	Q3Group_RemovePosition(Document.fModel, position);

	TQ3GeometryObject geometryObject = NewGamutObject(TRUE/*bDidGamutCompression*/);
	if (!geometryObject)
		return MCO_QD3D_ERROR;

	Q3Group_AddObject(Document.fModel, geometryObject);
	Q3Object_Dispose(geometryObject);

	return MCO_SUCCESS;
} 

/////////////////////////////////////////////////////////////////////////////
TQ3GeometryObject CQd3dGamutTweak::NewGamutObject(BOOL bDidGamutCompression)
{
	McoStatus status;

	double* gamutSurface = (double*)McoLockHandle(_hGamutData);
	if (!gamutSurface)
		return NULL;
	
	if (!_indexlut)
	{
		status = buildIndexLut(gamutSurface);
		if (status != MCO_SUCCESS)
		{
			McoUnlockHandle(_hGamutData);	
			return NULL;
		}
	}
	
	float uuMin =    0.0;
	float uuMax =  360.0;
	float uuStep =   9.0;
		
	// Create a mesh 
	TQ3GeometryObject geometryObject = Q3Mesh_New();
	if (!geometryObject)
		return NULL;

	TQ3Vertex3D* vertices = (TQ3Vertex3D*)malloc(500 * sizeof(TQ3Vertex3D));
	if (!vertices)
		return NULL;
	
	TQ3MeshVertex* meshVertices = (TQ3MeshVertex*)malloc(500 * sizeof(TQ3MeshVertex));
	if (!meshVertices)
		return NULL;
	
	double Lslice = _SliderL;
	long j = (int)(Lslice*2.56);
	
	// build all of the vertices
	double lab[4];
	lab[0] = Lslice;
	lab[1] = 0;
	lab[2] = 0;
	
	long i = 0;
	status = AddVertex(i,lab,vertices,meshVertices,geometryObject);
	if (status != MCO_SUCCESS)
		goto bail;
	i++;
				
	int in;
	for (in = 2; in <4; in++)
		{
		for (float uuValue = uuMax; uuValue > uuMin; uuValue -= uuStep)
			{
			long k2 = uuValue;		
			if (k2 >= 360) k2 = k2-360;
				
			if (bDidGamutCompression) 
				{
				if ((Lslice >= _minL) && (Lslice <= _maxL))
					{	
					long j = getgamutSurfaceIndex(Lslice,k2);	
					lab[0] = gamutSurface[j*4+k2*4*256];
					lab[1] = 1.05*(in*gamutSurface[j*4+k2*4*256+1])/3.0;
					lab[2] = 1.05*(in*gamutSurface[j*4+k2*4*256+2])/3.0;
					}
				else
					{
					lab[0] = Lslice;
					lab[1] = 0;
					lab[2] = 0;
					}
				} 
			else 
				{
				TQ3PolarPoint ppoint;
				ppoint.r = in*42.6;
				ppoint.theta = 0.01745329252*k2;
				TQ3Point2D point;
				Q3PolarPoint_ToPoint2D(&ppoint,&point);

				lab[0] = _SliderL;
				lab[1] = point.x;
				lab[2] = point.y;
				}
					
			status = AddVertex(i,lab,vertices,meshVertices,geometryObject);	
			if (status)
				goto bail;			
			i++;	
			}
		}

	// build the faces
	TQ3MeshVertex tempmeshVertices[4];
	for (in = 2; in <4; in++)
		{
		long m;
		long c = 1;
		for (float uuValue = uuMax; uuValue > uuMin; uuValue -= uuStep)
			{
			i = 0;
			if (in == 2)
				{
				tempmeshVertices[i++] = meshVertices[0];
				}
			else 
				{
				if (c==uuMax/uuStep)
					m = 1;
				else
					m = c+1;
				tempmeshVertices[i++] = meshVertices[m];
				m = c;
				tempmeshVertices[i++] = meshVertices[m];	
				}
			
			m = c+(in-2)*uuMax/uuStep;
			tempmeshVertices[i++] = meshVertices[m];
			if (c==uuMax/uuStep)
				m = 1+(in-2)*uuMax/uuStep;
			else
				m = 1+c+(in-2)*uuMax/uuStep;	 
			tempmeshVertices[i++] = meshVertices[m];
			c++;
			TQ3MeshFace meshFace = Q3Mesh_FaceNew(geometryObject, i, tempmeshVertices, NULL);
			}
		}	

	free((char *) vertices);
	vertices = NULL;
	free((char *) meshVertices);
	meshVertices = NULL;
	McoUnlockHandle(_hGamutData);	
	return geometryObject;
		
bail:
	if (vertices)
		free((char *) vertices);
	if (meshVertices)
		free((char *) meshVertices);
	McoUnlockHandle(_hGamutData);
	return NULL;
} 

/////////////////////////////////////////////////////////////////////////////
McoStatus CQd3dGamutTweak::AddVertex(int i, double *lab, TQ3Vertex3D *vertices, TQ3MeshVertex *meshVertices, TQ3GeometryObject geometryObject)
{
	double rgb[3];
	//j ConvertToRGB(lab,rgb);
	unsigned char crgb[3];
	labtorgb(lab, crgb, 1.8/*gamma*/);
	rgb[0] = crgb[0];
	rgb[1] = crgb[1];
	rgb[2] = crgb[2];

	TQ3ColorRGB color;
	color.r = rgb[0]/256;
	color.g = rgb[1]/256;
	color.b = rgb[2]/256; 

	vertices[i].point.x = lab[1]/128.0; 
	vertices[i].point.y = lab[2]/128.0; 
	vertices[i].point.z = 0.5; 		
	vertices[i].attributeSet = Q3AttributeSet_New();
	TQ3Status qd3_error = Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeDiffuseColor, &color);
	if (qd3_error != kQ3Success)
		return MCO_FAILURE;
	
	meshVertices[i] = Q3Mesh_VertexNew(geometryObject, &vertices[i]);		
	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
inline long CQd3dGamutTweak::getgamutSurfaceIndex(double L,double h)
{
	if (!_indexlut)
		return 0;

	short L_i = (short)(L*2.55);
	short h_i = (short)h;
	return _indexlut[h_i*256+L_i];
}

/////////////////////////////////////////////////////////////////////////////
// build a set of luts that is used to find the index into a gamutsurface
McoStatus CQd3dGamutTweak::buildIndexLut(double *gamutSurface)
{
	_indexlut = new short[256*360];
	if (!_indexlut)
		return MCO_MEM_ALLOC_ERROR;

	_minL = 101;
	_maxL = -1;

	for (int i=0; i<360; i++)
	{
		double* gp1 = &gamutSurface[i*256*4];
		double* gp2 = &gamutSurface[i*256*4+4];
		for (int j=0; j<256; j++)
		{
			double L = j/2.55;
			double* g1 = gp1;
			double* g2 = gp2;
			for (int k=0; k<255; k++)
			{
				if (*g1 > L) break;
				if ((*g1 < L) && (*g2 >= L)) break;
				g1 += 4;
				g2 += 4;
			}
			if (k >= 255) _indexlut[i*256+j] = 255;
			else if ( (L-*g1) < (*g2-L) ) _indexlut[i*256+j] = k;
			else _indexlut[i*256+j] = k+1;
		}
		for (j=0; j<255; j++)
		{
			if (gamutSurface[i*256*4+j*4]< _minL) _minL = gamutSurface[i*256*4+j*4];
			if (gamutSurface[i*256*4+j*4]> _maxL) _maxL = gamutSurface[i*256*4+j*4];
		}
	}
		
	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// build a line set that is the xy axies
TQ3GeometryObject CQd3dGamutTweak::New_ab_ax(void)
{
	TQ3GeometryObject	geometryObject = NULL;
	TQ3PolyLineData 	linedata;
	TQ3Vertex3D			*vertices = NULL;
	long i;
	TQ3Status			error;

	TQ3GroupObject group = Q3DisplayGroup_New ();

	linedata.numVertices = 2;
	linedata.vertices = (TQ3Vertex3D *) malloc(3 * sizeof(TQ3Vertex3D));
	if (linedata.vertices == NULL)	return NULL;

	vertices = 	linedata.vertices;
		
	linedata.segmentAttributeSet = NULL;
	linedata.polyLineAttributeSet = NULL;

	linedata.vertices[0].point.x = 1.0;
	linedata.vertices[0].point.y = 0.0;
	linedata.vertices[0].point.z = 0.5;
	linedata.vertices[0].attributeSet = NULL;

	linedata.vertices[1].point.x = -1.0;
	linedata.vertices[1].point.y = 0.0;
	linedata.vertices[1].point.z = 0.5;
	linedata.vertices[1].attributeSet = NULL;


	geometryObject = Q3PolyLine_New(&linedata);
	if (!geometryObject) goto bail;
	Q3Group_AddObject (group, geometryObject);
	error = Q3Object_Dispose(geometryObject);	

	if (error != kQ3Success) goto bail;

	linedata.numVertices = 2;

	linedata.vertices[0].point.x = 0.0;
	linedata.vertices[0].point.y = 1.0;
	linedata.vertices[0].point.z = 0.5;
	linedata.vertices[0].attributeSet = NULL;

	linedata.vertices[1].point.x = 0.0;
	linedata.vertices[1].point.y = -1.0;
	linedata.vertices[1].point.z = 0.5;
	linedata.vertices[1].attributeSet = NULL;

	geometryObject = Q3PolyLine_New(&linedata);
	if (!geometryObject) goto bail;
	Q3Group_AddObject (group, geometryObject);
	error = Q3Object_Dispose(geometryObject);

	if (error != kQ3Success) goto bail;
		
	free((char *) linedata.vertices);
	vertices = NULL;

	return	group;

bail:

	if (vertices != NULL) free((char *) vertices);
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
McoStatus CQd3dGamutTweak::ModifyCircle(void)
{
	double* lab1;
	double Lr;
	double Cr;

	if (!_pTweak)
	{
		lab1 = (double*)klab1;
		Lr = 10;
		Cr = 10;
	}
	else
	{
		Lr = _pTweak->Lr;
		Cr = _pTweak->Cr;
		if (_pTweak->type == Calibrate_Tweak)
			lab1 = _pTweak->lab_g;
		else
		if (_pTweak->type == GamutComp_Tweak)
			lab1 = _pTweak->lab_d;
	}

	long i;
	double	angle;
	double	astep = CIRCLE_STEP;
	double	sradius,radius;
	double	Ldist;
	TQ3ColorRGB 		color  = { 1.0, 0.0, 0.0 };
	TQ3ColorRGB			bcolor  = { 0.0, 0.0, 0.0 };
	TQ3Status			status;
	TQ3GroupPosition	position;
	TQ3Point3D	point;
	TQ3GeometryObject	object;
	double	new_Lr,new_Cr;

	// first get the circle

	status = Q3Group_GetFirstPositionOfType(Document.fModel, kQ3GeometryTypePolyLine, &position);

	status = Q3Group_GetPositionObject(Document.fModel, position, &object);

	Ldist = lab1[0] - _SliderL;
	Ldist = fabs(Ldist);
	if (Ldist > Lr) 
	{
		i = 0;
		for (angle = 0; angle <=360; angle += astep)
			{
			point.x = 0;
			point.y = 0;
			point.z = 0.6;
			Q3PolyLine_SetVertexPosition(object, i, &point);
			i++;
			}
		return MCO_SUCCESS;
	}

	sradius = (1-Ldist/Lr)*Cr+Ldist;

	radius = 0.01*sqrt(sradius*sradius-Ldist*Ldist);

	if (Lr == 0) return MCO_FAILURE;
	if (Cr == 0) return MCO_FAILURE;

	Cr = Cr * 2.55;

	new_Lr = (12500*0.00008*100+100*(Lr*Lr)-12500*2.995732274)/(Lr*Lr);
	new_Cr = (12500*0.00008*100+100*(Cr*Cr)-12500*2.995732274)/(Cr*Cr);

	radius = 0.0078125*sqrt((-0.00008*(100-new_Lr)*Ldist*Ldist+2.995732274)/(0.00008*(100-new_Cr)));


	i = 0;
	for (angle = 0; angle <=360; angle += astep)
	{
		point.x = lab1[1]/128.0+uMath_Cos_Deg(angle) * radius;
		point.y = lab1[2]/128.0+uMath_Sin_Deg(angle) * radius;
		point.z = 0.5;
		Q3PolyLine_SetVertexPosition(object, i, &point);
		i++;
	}

	Q3Object_Dispose(object);

	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// build a line set that is the xy axies
TQ3GeometryObject CQd3dGamutTweak::NewCircle(void)
{
	double* lab1;
	double Lr;
	double Cr;

	if (!_pTweak)
	{
		lab1 = (double*)klab1;
		Lr = 10;
		Cr = 10;
	}
	else
	{
		Lr = _pTweak->Lr;
		Cr = _pTweak->Cr;
		if (_pTweak->type == Calibrate_Tweak)
			lab1 = _pTweak->lab_g;
		else
		if (_pTweak->type == GamutComp_Tweak)
			lab1 = _pTweak->lab_d;
	}

	TQ3GeometryObject	geometryObject = NULL;
	TQ3PolyLineData 	linedata;
	TQ3Vertex3D			*vertices = NULL;
	long 				i,steps;
	TQ3GroupObject 		group;		
	TQ3ColorRGB 		color  = { 1.0, 0.0, 0.0 };

	steps = 1+360/CIRCLE_STEP;

	linedata.numVertices = steps;
	linedata.vertices = (TQ3Vertex3D *) malloc(steps * sizeof(TQ3Vertex3D));
	if (!linedata.vertices)	return NULL;
		
	linedata.segmentAttributeSet = NULL;
	linedata.polyLineAttributeSet = NULL;

	vertices = 	linedata.vertices;

	SetUpCircle(&linedata, lab1, Lr, Cr, _SliderL);
		
	geometryObject = Q3PolyLine_New(&linedata);

	free((char *) linedata.vertices);
	vertices = NULL;

	return	geometryObject;
}

/////////////////////////////////////////////////////////////////////////////
short CQd3dGamutTweak::SetUpCircle(TQ3PolyLineData *linedata,double *lab1,double Lr,double Cr,double currentL)
{
	long i;
	double	angle;
	double	astep = CIRCLE_STEP;
	double	sradius,radius;
	double	Ldist;
	TQ3ColorRGB 		color  = { 1.0, 1.0, 0.0 };
	TQ3ColorRGB			bcolor  = { 0.0, 0.0, 0.0 };
	double	new_Lr,new_Cr;

	Ldist = lab1[0]-currentL;
	Ldist = fabs(Ldist);
	if (Ldist > Lr) 
		{

		i = 0;
		for (angle = 0; angle <=360; angle += astep)
			{
			linedata->vertices[i].point.x = 0;
			linedata->vertices[i].point.y = 0;
			linedata->vertices[i].point.z = 0.6;
			linedata->vertices[i].attributeSet = Q3AttributeSet_New();
			Q3AttributeSet_Add(linedata->vertices[i].attributeSet, kQ3AttributeTypeDiffuseColor, &color);

			i++;
			}
		return 0;
		}
	sradius = (1-Ldist/Lr)*Cr+Ldist;

	radius = 0.01*sqrt(sradius*sradius-Ldist*Ldist);

	// -2.995732274 = ln(0.05)

	if (Lr == 0) Lr = 1;
	if (Cr == 0) Cr = 1;

	Cr = Cr * 2.55;

	new_Lr = (12500*0.00008*100+100*(Lr*Lr)-12500*2.995732274)/(Lr*Lr);
	new_Cr = (12500*0.00008*100+100*(Cr*Cr)-12500*2.995732274)/(Cr*Cr);

	radius = 0.0078125*sqrt((-0.00008*(100-new_Lr)*Ldist*Ldist+2.995732274)/(0.00008*(100-new_Cr)));

	i = 0;
	for (angle = 0; angle <=360; angle += astep)
	{
		linedata->vertices[i].point.x = lab1[1]/128.0+uMath_Cos_Deg(angle) * radius;
		linedata->vertices[i].point.y = lab1[2]/128.0+uMath_Sin_Deg(angle) * radius;
		linedata->vertices[i].point.z = 0.5;
		linedata->vertices[i].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(linedata->vertices[i].attributeSet, kQ3AttributeTypeDiffuseColor, &color);

		i++;
	}

	return 1;
}

/////////////////////////////////////////////////////////////////////////////
McoStatus CQd3dGamutTweak::ModifyArrow(void)
{
	double* lab1;
	double* lab2;

	if (!_pTweak)
	{
		lab1 = (double*)klab1;
		lab2 = (double*)klab2;
	}
	else
	if (_pTweak->type == Calibrate_Tweak)
	{
		lab1 = _pTweak->lab_g;
		lab2 = _pTweak->lab_p;
	}
	else
	if (_pTweak->type == GamutComp_Tweak)
	{
		lab1 = _pTweak->lab_d;
		lab2 = _pTweak->lab_g;
	}
		
	TQ3Status			status;
	TQ3GeometryObject	geometryObject = NULL;
	TQ3PolyLineData 	linedata;
	TQ3Vertex3D			*vertices = NULL;
	long i;
	TQ3GroupObject 		ggroup;	
	double				d,x,y;
	TQ3Point2D			point,p1,p2;
	TQ3PolarPoint		ppoint,pp1,pp2;

	TQ3GroupPosition	position;
	TQ3Point3D	point3;
	TQ3GeometryObject	object;

	// first get the line

	status = Q3Group_GetFirstPositionOfType(Document.fModel, kQ3ShapeTypeGroup, &position);
	if (!position) return MCO_FAILURE;
	status = Q3Group_GetNextPositionOfType(Document.fModel, kQ3ShapeTypeGroup, &position);
	if (!position) return MCO_FAILURE;
	status = Q3Group_GetPositionObject(Document.fModel, position, &ggroup);
	if (!ggroup) return MCO_FAILURE;

	status = Q3Group_GetFirstPositionOfType(ggroup, kQ3GeometryTypePolyLine, &position);
	if (!position) return MCO_FAILURE;

	status = Q3Group_GetPositionObject(ggroup, position, &object);
	if (!object) return MCO_FAILURE;

	x = (lab2[1]-lab1[1]);
	y = (lab2[2]-lab1[2]);
	d = sqrt(x*x+y*y);

	if (d == 0)
	{
		point3.x = lab1[1]/128.0;
		point3.y = lab1[2]/128.0;
		point3.z = 0.51;
		
		Q3PolyLine_SetVertexPosition(object, 0, &point3);

		point3.x = lab2[1]/128.0;
		point3.y = lab2[2]/128.0;
		point3.z = 0.51;
		
		Q3PolyLine_SetVertexPosition(object, 1, &point3);
		
		Q3Object_Dispose(object);
		status = Q3Group_GetNextPositionOfType(ggroup, kQ3GeometryTypePolyLine, &position);
		status = Q3Group_GetPositionObject(ggroup, position, &object);

		point3.x = (lab1[1])/128.0;
		point3.y = (lab1[2])/128.0;
		point3.z = 0.51;
		Q3PolyLine_SetVertexPosition(object, 0, &point3);

		point3.x = (lab1[1])/128.0;
		point3.y = (lab1[2])/128.0;
		point3.z = 0.51;
		Q3PolyLine_SetVertexPosition(object, 1, &point3);

		point3.x = (lab1[1])/128.0;
		point3.y = (lab1[2])/128.0;
		point3.z = 0.51;
		Q3PolyLine_SetVertexPosition(object, 2, &point3);
	}
	else 
	{
		point3.x = lab1[1]/128.0;
		point3.y = lab1[2]/128.0;
		point3.z = 0.51;
		Q3PolyLine_SetVertexPosition(object, 0, &point3);

		point3.x = (lab1[1]+x)/128.0;
		point3.y = (lab1[2]+y)/128.0;
		point3.z = 0.51;
		Q3PolyLine_SetVertexPosition(object, 1, &point3);

		// make the point

		point.x = x;
		point.y = y;

		Q3Point2D_ToPolar(&point, &ppoint);
		pp1 = ppoint;
		pp1.r *=0.9;
		pp1.theta += 0.1570796327;

		pp2 = ppoint;
		pp2.r *=0.9;
		pp2.theta -= 0.1570796327;

		Q3PolarPoint_ToPoint2D(&pp1,&p1);
		Q3PolarPoint_ToPoint2D(&pp2,&p2);
		
		Q3Object_Dispose(object);
		status = Q3Group_GetNextPositionOfType(ggroup, kQ3GeometryTypePolyLine, &position);
		status = Q3Group_GetPositionObject(ggroup, position, &object);


		point3.x = (lab1[1]+p1.x)/128.0;
		point3.y = (lab1[2]+p1.y)/128.0;
		point3.z = 0.51;
		Q3PolyLine_SetVertexPosition(object, 0, &point3);

		point3.x = (lab1[1]+x)/128.0;
		point3.y = (lab1[2]+y)/128.0;
		point3.z = 0.51;
		Q3PolyLine_SetVertexPosition(object, 1, &point3);

		point3.x = (lab1[1]+p2.x)/128.0;
		point3.y = (lab1[2]+p2.y)/128.0;
		point3.z = 0.51;
		Q3PolyLine_SetVertexPosition(object, 2, &point3);
	}
		
	Q3Object_Dispose(ggroup);
	Q3Object_Dispose(object);		
	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
TQ3GeometryObject CQd3dGamutTweak::NewArrow(void)
{
	double* lab1;
	double* lab2;

	if (!_pTweak)
	{
		lab1 = (double*)klab1;
		lab2 = (double*)klab2;
	}
	else
	if (_pTweak->type == Calibrate_Tweak)
	{
		lab1 = _pTweak->lab_g;
		lab2 = _pTweak->lab_p;
	}
	else
	if (_pTweak->type == GamutComp_Tweak)
	{
		lab1 = _pTweak->lab_d;
		lab2 = _pTweak->lab_g;
	}

	TQ3GeometryObject	geometryObject = NULL;
	TQ3PolyLineData 	linedata;
	TQ3Vertex3D			*vertices = NULL;
	long i;
	TQ3GroupObject 		group;	
	double				d,x,y;
	TQ3Point2D			point,p1,p2;
	TQ3PolarPoint		ppoint,pp1,pp2;
	TQ3ColorRGB 		color  = { 1.0, 1.0, 1.0 };
	TQ3Status			error;

	group = Q3DisplayGroup_New ();
	if (!group) return 0L;

	x = (lab2[1]-lab1[1]);
	y = (lab2[2]-lab1[2]);
	d = sqrt(x*x+y*y);

	if (d == 0)
	{
		linedata.numVertices = 2;
		linedata.vertices = (TQ3Vertex3D *) malloc(3 * sizeof(TQ3Vertex3D));
			if (linedata.vertices == NULL)	return NULL;

		vertices = 	linedata.vertices;
			
		linedata.segmentAttributeSet = NULL;
		linedata.polyLineAttributeSet = NULL;

		linedata.vertices[0].point.x = lab1[1]/128.0;
		linedata.vertices[0].point.y = lab1[2]/128.0;
		linedata.vertices[0].point.z = 0.51;
		linedata.vertices[0].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(linedata.vertices[0].attributeSet, kQ3AttributeTypeDiffuseColor, &color);
		

		linedata.vertices[1].point.x = lab2[1]/128.0;
		linedata.vertices[1].point.y = lab2[2]/128.0;
		linedata.vertices[1].point.z = 0.51;
		linedata.vertices[1].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(linedata.vertices[1].attributeSet, kQ3AttributeTypeDiffuseColor, &color);


		geometryObject = Q3PolyLine_New(&linedata);
		if (!geometryObject) goto bail;
		Q3Group_AddObject (group, geometryObject);
		error = Q3Object_Dispose(geometryObject);	
		if (error != kQ3Success) goto bail;
		
		linedata.numVertices = 3;

		linedata.vertices[0].point.x = (lab1[1])/128.0;
		linedata.vertices[0].point.y = (lab1[2])/128.0;
		linedata.vertices[0].point.z = 0.51;
		linedata.vertices[0].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(linedata.vertices[0].attributeSet, kQ3AttributeTypeDiffuseColor, &color);

		linedata.vertices[1].point.x = (lab1[1])/128.0;
		linedata.vertices[1].point.y = (lab1[2])/128.0;
		linedata.vertices[1].point.z = 0.51;
		linedata.vertices[1].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(linedata.vertices[0].attributeSet, kQ3AttributeTypeDiffuseColor, &color);

		linedata.vertices[2].point.x = (lab1[1])/128.0;
		linedata.vertices[2].point.y = (lab1[2])/128.0;
		linedata.vertices[2].point.z = 0.51;
		linedata.vertices[2].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(linedata.vertices[2].attributeSet, kQ3AttributeTypeDiffuseColor, &color);

		geometryObject = Q3PolyLine_New(&linedata);
		if (!geometryObject) goto bail;
		Q3Group_AddObject (group, geometryObject);
		error = Q3Object_Dispose(geometryObject);
		if (error != kQ3Success) goto bail;
	}
	else 
	{
		x = 1.0*x/d;
		y = 1.0*y/d;

		// make the line

		linedata.numVertices = 2;
		linedata.vertices = (TQ3Vertex3D *) malloc(3 * sizeof(TQ3Vertex3D));
			if (linedata.vertices == NULL)	return NULL;

		vertices = 	linedata.vertices;
			
		linedata.segmentAttributeSet = NULL;
		linedata.polyLineAttributeSet = NULL;

		linedata.vertices[0].point.x = lab1[1]/128.0;
		linedata.vertices[0].point.y = lab1[2]/128.0;
		linedata.vertices[0].point.z = 0.51;
		linedata.vertices[0].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(linedata.vertices[0].attributeSet, kQ3AttributeTypeDiffuseColor, &color);

		linedata.vertices[1].point.x = (lab1[1]+x)/128.0;
		linedata.vertices[1].point.y = (lab1[2]+y)/128.0;
		linedata.vertices[1].point.z = 0.51;
		linedata.vertices[1].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(linedata.vertices[1].attributeSet, kQ3AttributeTypeDiffuseColor, &color);


		geometryObject = Q3PolyLine_New(&linedata);
		if (!geometryObject) goto bail;
		Q3Group_AddObject (group, geometryObject);
		error = Q3Object_Dispose(geometryObject);	

		if (error != kQ3Success) goto bail;
		// make the point

		point.x = x;
		point.y = y;

		Q3Point2D_ToPolar(&point, &ppoint);
		pp1 = ppoint;
		pp1.r *=0.9;
		pp1.theta += 0.1570796327;

		pp2 = ppoint;
		pp2.r *=0.9;
		pp2.theta -= 0.1570796327;

		Q3PolarPoint_ToPoint2D(&pp1,&p1);
		Q3PolarPoint_ToPoint2D(&pp2,&p2);

		linedata.numVertices = 3;

		linedata.vertices[0].point.x = (lab1[1]+p1.x)/128.0;
		linedata.vertices[0].point.y = (lab1[2]+p1.y)/128.0;
		linedata.vertices[0].point.z = 0.51;
		linedata.vertices[0].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(linedata.vertices[0].attributeSet, kQ3AttributeTypeDiffuseColor, &color);

		linedata.vertices[1].point.x = (lab1[1]+x)/128.0;
		linedata.vertices[1].point.y = (lab1[2]+y)/128.0;
		linedata.vertices[1].point.z = 0.51;
		linedata.vertices[1].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(linedata.vertices[1].attributeSet, kQ3AttributeTypeDiffuseColor, &color);

		linedata.vertices[2].point.x = (lab1[1]+p2.x)/128.0;
		linedata.vertices[2].point.y = (lab1[2]+p2.y)/128.0;
		linedata.vertices[2].point.z = 0.51;
		linedata.vertices[2].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(linedata.vertices[2].attributeSet, kQ3AttributeTypeDiffuseColor, &color);

		geometryObject = Q3PolyLine_New(&linedata);
		if (!geometryObject) goto bail;
		Q3Group_AddObject (group, geometryObject);
		error = Q3Object_Dispose(geometryObject);
		if (error != kQ3Success) goto bail;
	}

	free((char *) linedata.vertices);
	vertices = NULL;

	return	group;

	bail:

	if (vertices != NULL) free((char *) vertices);
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
TQ3GeometryObject CQd3dGamutTweak::NewPolyy(double *disp,double *polypoints,double scale,int num_pp,TQ3ColorRGB *color)
{
	TQ3GeometryObject	geometryObject = NULL;
	TQ3GeneralPolygonData 		poly;
	TQ3Vertex3D			*vertices;
	int i;

	poly.numContours = 1;
	poly.contours = new TQ3GeneralPolygonContourData;
	poly.shapeHint = kQ3GeneralPolygonShapeHintConvex;
	poly.contours[0].numVertices = num_pp;
	poly.contours[0].vertices = new TQ3Vertex3D [num_pp];
	if (poly.contours[0].vertices == NULL)	return NULL;
		
	vertices = 	poly.contours[0].vertices;

	poly.generalPolygonAttributeSet = Q3AttributeSet_New();;
	Q3AttributeSet_Add(poly.generalPolygonAttributeSet, kQ3AttributeTypeDiffuseColor, color);

	for (i=0; i<num_pp; i++)
	{
		vertices[i].point.x = polypoints[i*2]*scale+disp[0];
		vertices[i].point.y = polypoints[i*2+1]*scale+disp[1];
		vertices[i].point.z = disp[2];
		vertices[i].attributeSet = 0L;
	}
		
	geometryObject = Q3GeneralPolygon_New(&poly);

	Q3Object_Dispose(poly.generalPolygonAttributeSet);	
		
	delete vertices;
	delete poly.contours;
	return	geometryObject;	
}		


/////////////////////////////////////////////////////////////////////////////
McoStatus CQd3dGamutTweak::ModifyPoly(double *disp,double *polypoints,double scale,int num_pp)
{
	long i;
	double	angle;
	double	astep = CIRCLE_STEP;
	double	sradius,radius;
	double	Ldist;
	TQ3Status			status;
	TQ3GroupPosition	position;
	TQ3Point3D	point;
	TQ3GeometryObject	object;

	// first get the circle

	status = Q3Group_GetFirstPositionOfType(Document.fModel, kQ3GeometryTypeGeneralPolygon, &position);
	if (!position) return MCO_FAILURE;

	status = Q3Group_GetPositionObject(Document.fModel, position, &object);
	if (!object) return MCO_FAILURE;

	for (i=0; i<num_pp; i++)
	{
		point.x = polypoints[i*2]*scale+disp[0];
		point.y = polypoints[i*2+1]*scale+disp[1];
		point.z = disp[2];
		status = Q3GeneralPolygon_SetVertexPosition(object, 0, i, &point);
	}
		
	Q3Object_Dispose(object);	
	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
TQ3GeometryObject CQd3dGamutTweak::MakeCross(BOOL bNew)
{
	double* lab1;

	if (!_pTweak)
		lab1 = (double*)klab1;
	else
		lab1 = _pTweak->lab_d;

	double CrossPoly[] = {
		-0.5, -1.5,
		 0.5, -1.5,
		 0.5, -0.5,
		 1.5, -0.5,
		 1.5,  0.5,
		 0.5,  0.5,
		 0.5,  1.5,
		-0.5,  1.5,
		-0.5,  0.5,
		-1.5,  0.5,
		-1.5, -0.5,
		-0.5, -0.5
	};
		
	double disp[3];
	disp[0] = lab1[1]/128;
	disp[1] = lab1[2]/128;
	disp[2] = 0.51;

	double scale = 3*Document.fGroupScale;
	if (scale < 1) scale = 1;
		
	if (bNew)
	{
		TQ3ColorRGB color = { 1.0, 1.0, 1.0 };
		return NewPolyy(disp, CrossPoly, 0.02/scale, 12, &color);
	}
	else
	{
		McoStatus status = ModifyPoly(disp, CrossPoly, 0.02/scale, 12);
		return NULL;
	}
}

#ifdef NOTUSED
/////////////////////////////////////////////////////////////////////////////
int CQd3dGamutTweak::isColorInGamut(double *lab)
{
	if (lab[1] < -128) lab[1] = -128;
	if (lab[1] > 127) lab[1] = 127;

	if (lab[2] < -128) lab[2] = -128;
	if (lab[2] > 127) lab[2] = 127;

	int j = 33*lab[0]/100;
	int k = 33*(lab[1]+128)/256;
	int l = 33*(lab[2]+128)/256;

	unsigned char*out_gamut = (unsigned char*)McoLockHandle(doc->_out_of_gamutH);

	int gamut;
	if (out_gamut[l+k*33+j*33*33])
		gamut = 0;
	else
		gamut = 1;

	McoUnlockHandle(doc->_out_of_gamutH);
	return gamut;
}
#endif NOTUSED

