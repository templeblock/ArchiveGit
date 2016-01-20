#include "stdafx.h"
#include "Qd3dGamutView.h"
#include "Qd3dExtras.h"
#include "xyztorgb.h"
#include "MathUtilities.h"
#include "colortran.h"

/////////////////////////////////////////////////////////////////////////////
CQd3dGamutView::CQd3dGamutView()
{
}

/////////////////////////////////////////////////////////////////////////////
CQd3dGamutView::~CQd3dGamutView()
{
}		

/////////////////////////////////////////////////////////////////////////////
int /*iFOVMultiplier*/ CQd3dGamutView::CreateGeometry(void)
{
	McoStatus status;

	status = ChangeGeometry(MP_GammutSurface,0);
	if (status != MCO_SUCCESS)
		;
			
	status = ChangeGeometry(MP_CIELabRef_PG,0);
	if (status != MCO_SUCCESS)
		;
	
	status = ChangeGeometry(MP_LxyAx,0);
	if (status != MCO_SUCCESS)
		;

	return 2; /*iFOVMultiplier*/
}

/////////////////////////////////////////////////////////////////////////////
McoStatus CQd3dGamutView::ChangeGeometry(Qd3dType type, long num)
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
		case MP_GammutSurface:
			object = NewGamutObject();
			break;
		case MP_CIELabRef_PG:
			object = NewPolyGCIELabRef();
			break;
		case MP_CIELabRef_PL:
			object = NewGamutObject();
			break;
		case MP_LxyAx:
			object = NewLxyAx();
			break;
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
TQ3GeometryObject CQd3dGamutView::NewGamutObject(void)
{
	TQ3Error qd3_error;

	double* gamutSurface = (double*)McoLockHandle(_hGamutData);
	
	// uuValue is an angle used to revolve the cross section about the y axis
	// vvValue is an angle used to generate each half circular cross section
	 
	float uuMin =   0.0;
	float uuMax = 360.0;
	float vvMin = -99.0;
	float vvMax =  99.0;
	float uuStep =  9.0;
	float vvStep =  9.0;
	float radius =  1.0;

	// Setup TQ3TriGridData 
	TQ3TriGridData data;
	data.numRows		= (unsigned long)((vvMax - vvMin) / vvStep) + 1;
	data.numColumns		= (unsigned long)((uuMax - uuMin) / uuStep) + 1;
	data.facetAttributeSet	= NULL;
	data.triGridAttributeSet= NULL;

	TQ3Vertex3D* vertices = (TQ3Vertex3D *) malloc(data.numRows * data.numColumns * sizeof(TQ3Vertex3D));
	if (!vertices)
		return NULL;

	// Set trigrid vertices and shading UVs, if it hasUVAttributes 
	data.vertices = vertices;
	int i = 0;
	for (double vvValue = vvMin; vvValue <= vvMax; vvValue += vvStep)
	{
		unsigned long j, k;
		for (double uuValue = uuMax; uuValue >= uuMin; uuValue -= uuStep)
		{
		if ((vvValue>vvMin) && (vvValue<vvMax))
		{
			j = (unsigned long)(vvValue*1.416+127);
			k = (unsigned long)uuValue;
			if (k >= 360) k = 0;
			radius = (float)gamutSurface[j*4+k*4*256+3];
		}
		else if (vvValue==vvMin)
		{
			j = (unsigned long)((vvValue+vvStep)*1.416+127);
			k = 0;
			if (k >= 360) k = 0;
			radius = 0;
		}
		else 
		{
			j = (unsigned long)((vvValue-vvStep)*1.416+127);
			k = 0;
			if (k >= 360) k = 0;
			radius = 0;
		}

		//Add the specified color attribute to the attribute set.
		double lab[4];
		lab[0] = gamutSurface[j*4+k*4*256];
		lab[1] = gamutSurface[j*4+k*4*256+1];
		lab[2] = gamutSurface[j*4+k*4*256+2];
		labtonxyzinplace(lab, 1);
		
		double X, Y, Z;
		X = lab[0]*96.442;
		Y = lab[1]*100;
		Z = lab[2]*82.521;
		
		double x, y, z;
		if (X+Y+Z != 0)
		{
			x = X/(X+Y+Z);
			y = Y/(X+Y+Z);
			z = Z/(X+Y+Z);
		}
		else
		{
			x = 0.5;
			y = 0.5;
			z = 0;
		}

		vertices[i].point.x = (float)x; 
		vertices[i].point.y = (float)y; 
		vertices[i].point.z = (float)(gamutSurface[j*4+k*4*256]/100);

		McoStatus status = _XyztoRgb.compute(lab, _monitor_z, 1);
		if (status != MCO_SUCCESS)
			goto bail;

		TQ3ColorRGB color;
		color.r = (float)(lab[0]/256);
		color.g = (float)(lab[1]/256);
		color.b = (float)(lab[2]/256);
		vertices[i].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeDiffuseColor, &color);
		
		qd3_error = Q3Error_Get(NULL);
		if (qd3_error != kQ3ErrorNone) goto bail;
		
		TQ3Vertex3D norms;
		norms.point.x = uMath_Cos_Deg(vvValue) * uMath_Cos_Deg(uuValue) * (radius+2);
		norms.point.y = uMath_Sin_Deg(vvValue)*129;
		norms.point.z = uMath_Cos_Deg(vvValue) * uMath_Sin_Deg(uuValue) * (radius+2);
		
		Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeNormal, &norms);

		qd3_error = Q3Error_Get(NULL);
		if (qd3_error != kQ3ErrorNone) goto bail;

		i++;
		}
	}

	{
		TQ3GeometryObject geometryObject = Q3TriGrid_New(&data);
		qd3_error = Q3Error_Get(NULL);
		if (qd3_error != kQ3ErrorNone) goto bail;
		
		for (i = 0; i < data.numRows * data.numColumns; i++)
			Q3Object_Dispose(vertices[i].attributeSet);

		if (vertices)
			free((char *)vertices);
		
		McoUnlockHandle(_hGamutData);
		return	geometryObject;
	}

bail:
	if (vertices)
		free((char *)vertices);

	McoUnlockHandle(_hGamutData);
	return NULL;
} 

/////////////////////////////////////////////////////////////////////////////
#define NumCIELabRef (4)
double CIELabRef[] = {
	-128,-128,
	 128,-128,
	 128, 128,
	-128, 128
};

#define NumCIELuvRef  (12)	
double CIELuvRef[] = {
//	0.1694, 0.5025,
	0.260, 0.020,
	0.180, 0.085,
	0.090, 0.250,
	0.050, 0.350,
	0.005, 0.500,
	0.000, 0.550,
	0.020, 0.580,
	0.050, 0.590,
	0.100, 0.580,
	0.330, 0.550,
	0.625, 0.500,
	0.450, 0.275
};
	
#define NumCIExyzRef  (40)
double CIExyzRef[] = {	
	370, 0.1455, 0.0182, 0.8364,
	380, 0.1648, 0.0147, 0.8205,
	390, 0.1762, 0.0131, 0.8107,
	400, 0.1768, 0.0130, 0.8101,
	410, 0.1761, 0.0129, 0.8110,
	420, 0.1747, 0.0134, 0.8120,
	430, 0.1723, 0.0153, 0.8124,
	440, 0.1677, 0.0192, 0.8132,
	450, 0.1598, 0.0259, 0.8143,
	460, 0.1470, 0.0380, 0.8150,
	470, 0.1266, 0.0660, 0.8074,
	480, 0.0933, 0.1409, 0.7658,
	490, 0.0464, 0.3033, 0.6504,
	500, 0.0081, 0.5454, 0.4465,
	510, 0.0140, 0.7548, 0.2312,
	520, 0.0748, 0.8354, 0.0898,
	530, 0.1559, 0.8056, 0.0385,
	540, 0.2310, 0.7532, 0.0158,
	550, 0.3030, 0.6909, 0.0061,
	560, 0.3741, 0.6235, 0.0024,
	570, 0.4448, 0.5541, 0.0012,
	580, 0.5124, 0.4867, 0.0009,
	590, 0.5742, 0.4251, 0.0006,
	600, 0.6253, 0.3742, 0.0004,
	610, 0.6635, 0.3363, 0.0002,
	620, 0.6887, 0.3112, 0.0002,
	630, 0.7047, 0.2952, 0.0001,
	640, 0.7156, 0.2844, 0.0000,
	650, 0.7226, 0.2774, 0.0000,
	660, 0.7263, 0.2737, 0.0000,
	670, 0.7284, 0.2716, 0.0000,
	680, 0.7297, 0.2703, 0.0000,
	690, 0.7303, 0.2697, 0.0000,
	700, 0.7338, 0.2662, 0.0000,
	710, 0.7308, 0.2692, 0.0000,
	720, 0.7179, 0.2821, 0.0000,
	730, 0.7500, 0.2500, 0.0000,
	740, 0.7143, 0.2857, 0.0000,
	750, 0.7500, 0.2500, 0.0000,
	760, 0.6667, 0.3333, 0.0000
};

#define bigger(a, b) ((a >= b) ? a :b)
#define smaller(a, b) ((a < b) ? a :b)

/////////////////////////////////////////////////////////////////////////////
// build a polygon that is a CIELab reference
TQ3GeometryObject CQd3dGamutView::NewPolyGCIELabRef(void)
{
	TQ3PolygonData ciepoly;
	ciepoly.numVertices = NumCIExyzRef;

	TQ3Vertex3D* vertices = (TQ3Vertex3D*)malloc(NumCIExyzRef * sizeof(TQ3Vertex3D));
	if (!vertices)
		return NULL;
		
	ciepoly.vertices = 	vertices;
		
	ciepoly.polygonAttributeSet = NULL;
	for (long i=0; i<NumCIExyzRef; i++)
	{
		double Lxy[4];
		Lxy[0] = 50;
		Lxy[1] = CIExyzRef[i*4+1];
		Lxy[2] = CIExyzRef[i*4+2];
		
		vertices[i].point.x = (float)Lxy[1];
		vertices[i].point.y = (float)Lxy[2];
		vertices[i].point.z = 0.5;
		
		double lab[4];
		sxyztolab(Lxy,lab);
		
		double big_ab = bigger(fabs(lab[1]),fabs(lab[2]));
		if (big_ab>127)
		{
			lab[1] = lab[1]*127/big_ab;
			lab[2] = lab[2]*127/big_ab;
		}
			
		labtonxyzinplace(lab,1);

		McoStatus state = _XyztoRgb.compute(lab, _monitor_z, 1);
		if (state != MCO_SUCCESS)
			return NULL;	
		
		double big_rgb = bigger(lab[0],bigger(lab[1],lab[2]));
		if (big_rgb>255)
			{
			lab[0] = lab[0]*255/big_rgb;
			lab[1] = lab[1]*255/big_rgb;
			lab[2] = lab[2]*255/big_rgb;
			}

		TQ3ColorRGB color;
		color.r = (float)(lab[0]/256);
		color.g = (float)(lab[1]/256);
		color.b = (float)(lab[2]/256);
		vertices[i].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeDiffuseColor, &color);
		if (Q3Error_Get(NULL) != kQ3ErrorNone) goto bail;
	}
		
	{
		TQ3GeometryObject geometryObject = Q3Polygon_New(&ciepoly);
		if (Q3Error_Get(NULL) != kQ3ErrorNone) goto bail;

		for (i = 0; i < NumCIExyzRef; i++)
			Q3Object_Dispose(vertices[i].attributeSet);	
			
		if (vertices)
			free((char*)vertices);

		return	geometryObject;
	}

bail:
	if (vertices)
		free((char*)vertices);

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// build a poly-line that is a CIELab refrence
TQ3GeometryObject CQd3dGamutView::NewPolyLCIELabRef(void)
{
	TQ3PolyLineData ciepoly;
	ciepoly.numVertices = NumCIELabRef;

	TQ3Vertex3D* vertices = (TQ3Vertex3D *) malloc(NumCIELabRef * sizeof(TQ3Vertex3D));
	if (!vertices)
		return NULL;
		
	ciepoly.vertices = vertices;
		
	ciepoly.segmentAttributeSet = NULL;
	ciepoly.polyLineAttributeSet = NULL;
	for (long i=0; i<NumCIELabRef; i++)
	{
		vertices[i].point.x = (float)CIELabRef[i*2];
		vertices[i].point.y = 0;
		vertices[i].point.z = (float)CIELabRef[i*2];
	}

	TQ3GeometryObject geometryObject = Q3PolyLine_New(&ciepoly);
	if (Q3Error_Get(NULL) != kQ3ErrorNone) goto bail;
		
	if (vertices)
		free((char*)vertices);
	return	geometryObject;

bail:

	if (vertices)
		free((char*)vertices);
	return NULL;
}


static unsigned char MarkerBitmaps[3][16] = {
/* L */
{ 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFF },
/* x */
{ 0xC3, 0xC3, 0x66, 0x66, 0x18, 0x66, 0x66, 0xC3, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
/* y */
{ 0xC3, 0xC3, 0xC3, 0xC3, 0x66, 0x66, 0x66, 0x66, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 },
};

// Create a marker from a string
/////////////////////////////////////////////////////////////////////////////
TQ3GeometryObject MarkerFromText(int index, double x, double y, double z)
{
	// set up the marker
	TQ3MarkerData markerData;
	Q3Point3D_Set(&markerData.location, x, y, z);
	markerData.xOffset			= 0;
	markerData.yOffset			= 0;
	markerData.bitmap.image		= (unsigned char*)MarkerBitmaps[index]; // memory
	markerData.bitmap.width		= 8; // width
	markerData.bitmap.height	= 16; // height
	markerData.bitmap.rowBytes	= 1; // memoryRowBytes
	markerData.bitmap.bitOrder	= kQ3EndianLittle;
	markerData.markerAttributeSet = NULL;
	TQ3Object myMarker = Q3Marker_New(&markerData);
	return (myMarker);		
}

/////////////////////////////////////////////////////////////////////////////
// build a line set that is the xy axies
TQ3GeometryObject CQd3dGamutView::NewLxyAx(void)
{
	TQ3GroupObject group = Q3DisplayGroup_New();

	TQ3PolyLineData linedata;
	linedata.numVertices = 3;

	TQ3Vertex3D* vertices = (TQ3Vertex3D *) malloc(3 * sizeof(TQ3Vertex3D));
	if (!vertices)
		return NULL;

	linedata.vertices = vertices;
		
	linedata.segmentAttributeSet = NULL;
	linedata.polyLineAttributeSet = NULL;

	linedata.vertices[0].point.x = 1.0;
	linedata.vertices[0].point.y = 0.0;
	linedata.vertices[0].point.z = 0.0;
	linedata.vertices[0].attributeSet = NULL;

	linedata.vertices[1].point.x = 0.0;
	linedata.vertices[1].point.y = 0.0;
	linedata.vertices[1].point.z = 0.0;
	linedata.vertices[1].attributeSet = NULL;

	linedata.vertices[2].point.x = 0.0;
	linedata.vertices[2].point.y = 1.0;
	linedata.vertices[2].point.z = 0.0;
	linedata.vertices[2].attributeSet = NULL;

	TQ3GeometryObject geometryObject = Q3PolyLine_New(&linedata);
	Q3Group_AddObject(group, geometryObject);
	Q3Object_Dispose(geometryObject);	

	if (Q3Error_Get(NULL) != kQ3ErrorNone)
		goto bail;

	linedata.numVertices = 2;

	linedata.vertices[0].point.x = 0.0;
	linedata.vertices[0].point.y = 0.0;
	linedata.vertices[0].point.z = 0.0;
	linedata.vertices[0].attributeSet = NULL;

	linedata.vertices[1].point.x = 0.0;
	linedata.vertices[1].point.y = 0.0;
	linedata.vertices[1].point.z = 1.0;
	linedata.vertices[1].attributeSet = NULL;

	geometryObject = Q3PolyLine_New(&linedata);
	Q3Group_AddObject(group, geometryObject);
	Q3Object_Dispose(geometryObject);	

	if (Q3Error_Get(NULL) != kQ3ErrorNone)
		goto bail;

	geometryObject = MarkerFromText(1/*"x"*/,1,0,0);
	Q3Group_AddObject(group, geometryObject);
	Q3Object_Dispose(geometryObject);	
	if (Q3Error_Get(NULL) != kQ3ErrorNone)
		goto bail;

	geometryObject = MarkerFromText(2/*"y"*/,0,1,0);
	Q3Group_AddObject(group, geometryObject);
	Q3Object_Dispose(geometryObject);	
	if (Q3Error_Get(NULL) != kQ3ErrorNone)
		goto bail;

	geometryObject = MarkerFromText(0/*"L"*/,0,0,1);
	Q3Group_AddObject(group, geometryObject);
	Q3Object_Dispose(geometryObject);	
	if (Q3Error_Get(NULL) != kQ3ErrorNone)
		goto bail;

	if (vertices)
		free((char*)vertices);
	return	group;

bail:
	if (vertices)
		free((char*)vertices);
	return NULL;
}

