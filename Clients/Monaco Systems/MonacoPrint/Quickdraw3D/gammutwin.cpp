/*********************************************/
/* gammutwin.c							 	 */
/* Display 3D gammut surface in window		 */
/* Jan 19, 1996							     */
/*********************************************/
/*   James Vogh 							 */
/*********************************************/

#include <QD3DGeometry.h>
#include "gammutwin.h"
#include "mcostat.h"

#include <Memory.h>
#include "math.h"
#include "colortran.h"
#include "mcofile.h"

#include "AttributeSet_Lib.h"

#include "MathUtilities.h"
#include "QD3DPick.h"





TQ3GroupObject MyNewModel2();

// initialize the 3D and load an object
GamutWin::GamutWin(long s, long e, McoHandle gSH,int32 winnum,MS_WindowTypes wint,XyztoRgb 	*xtor,double mg,unsigned char *patchFName) 
{
	TQ3Status	myStatus;
	//Rect		rBounds;
	FSSpec		theFileSpec ;				// the file we are opening
	Str255 		title = "\p";
	McoStatus   status;
	Handle			item_handle;
	short			item_type;
	int			Q3DIDS[] = GS_IDS;
	int			i;
	//if (Q3Initialize == kUnresolvedSymbolAddress) goto fail;
	
	priority = 1;
	
	object_state = MCO_SUCCESS;
	
	WinNum = winnum;
	WinType = wint;
	
	ambient = 0.6;
	point = 0.8;
	fill = 0.2;
	
	Document.fCameraFrom.x = 0.0; 
	Document.fCameraFrom.y = 0.0;
	Document.fCameraFrom.z = 30.0;
	
	Document.fCameraTo.x = 0.0; 
	Document.fCameraTo.y = 0.0;
	Document.fCameraTo.z = 0.0;
	
	for (i=0; i < NUM_GMW_IDS; i++) ids[i] = Q3DIDS[i];

	setDialog(ids[QD3D_Dial]);
	the_window = dp;

	GetDItem (dp, ids[QD3D_Box], &item_type, &item_handle, &box_rect);

	// initialise our document structure
	InitDocumentData( &Document ) ;
	
	start = s;
	end = e;
	gammutSurfaceH = gSH;
	// try to read the file into the main display group
	num_objects = 0;
	xyztorgb = xtor;
	monitor_z = mg;
//	Document.fModel = MyNewModel2(); //NewPolyGCIELabRef() ;
	
	status = ChangeGeometry(MP_GammutSurface,0);
	if (status != MCO_SUCCESS) goto fail;
			
	status = ChangeGeometry(MP_CIELabRef_PG,0);
	if (status != MCO_SUCCESS) goto fail;
	
	status = ChangeGeometry(MP_LxyAx,0);
	if (status != MCO_SUCCESS) goto fail;
	
	AdjustCamera(	&Document,
					(box_rect.right - box_rect.left),
					(box_rect.bottom - box_rect.top) ) ;

	if (patchFName != NULL) SetWTitle( the_window, patchFName);
	ShowWindow( the_window );
	SetPort( the_window );

	DocumentDraw3DData(&Document);
	DocumentDrawOnscreen( &Document, &box_rect ) ;
	
	Current_Button = ids[QD3D_Rotate];
	Disable(dp,ids[QD3D_Rotate]);
	
	
	
	return;
fail:
//	delete this;
//	this = NULL;
	object_state = status;

}

GamutWin::~GamutWin(void) 
{
	TQ3Status	myStatus;
	//DisposeDocumentData( &Document ) ;

	//	Close our connection to the QuickDraw 3D library
	//CloseDialog(dp);
	//if (the_window != NULL) DisposeWindow(the_window);
}		
		
TQ3GroupObject MyNewModel2()
{
	TQ3GroupObject	myGroup = NULL;
	TQ3Object		myMarker = NULL;
	TQ3MarkerData	markerData;
	unsigned char	memory[1000];
	unsigned char*	ptr;
	long			i;
			
	// Create a group for the complete model.
	if ((myGroup = Q3DisplayGroup_New()) != NULL ) {
		
		// set up the marker
		Q3Point3D_Set(&markerData.location, 0.0, 0.0, 0.0);
		markerData.xOffset			= -2;
		markerData.yOffset			= -15;
		markerData.bitmap.image		= memory;
		markerData.bitmap.width		= 30;
		markerData.bitmap.height	= 30;
		markerData.bitmap.rowBytes	= Q3Bitmap_GetImageSize(30, 30) / 30;
		markerData.bitmap.bitOrder	= kQ3EndianBig;
		markerData.markerAttributeSet = NULL;
		
		// brute force method to set up the bitmap
		ptr = markerData.bitmap.image;
		for (i=0; i<8; i++)
			{
			*ptr++ = 0xff;
			*ptr++ = 0xff;
			*ptr++ = 0xff;
			*ptr++ = 0xff;
			}
		for (i=0; i<14; i++)
			{
			*ptr++ = 0xfc;
			*ptr++ = 0x00;
			*ptr++ = 0x00;
			*ptr++ = 0xff;
			}
		for (i=0; i<8; i++)
			{
			*ptr++ = 0xff;
			*ptr++ = 0xff;
			*ptr++ = 0xff;
			*ptr++ = 0xff;
			}
			
		// now create the marker
		myMarker = Q3Marker_New(&markerData);
		Q3Group_AddObject(myGroup, myMarker);	
		
		// dispose of the objects we created here
		if( myMarker ) 
			Q3Object_Dispose(myMarker);
	}
	
	//	Done!
	return ( myGroup );
}

/*
TQ3GeometryObject	GamutWin::NewGammutObject(long start, long end,McoHandle gammutSurfaceH)
{
	#define	kNumVertices	5
	#define	kAngleMax		360.0
	#define	kAngleIncr		(kAngleMax / kNumVertices)

	TQ3GeometryObject	geometry;
	unsigned long		i;
	float				angle,
						radius;
	TQ3Vertex3D			vertices[kNumVertices];
	TQ3MeshVertex		meshVertices[kNumVertices];
	TQ3MeshFace			meshFace;
	TQ3ColorRGB 		color  = { 0.0, 0.0, 0.0 };

	geometry = NULL;


	// Create a mesh 
	geometry = Q3Mesh_New();

	// Create a mesh face 
	radius = 1.0;
	for (i = 0, angle = kAngleIncr / 4.0;
		i < kNumVertices;
		i++, angle += kAngleIncr) {

		vertices[i].point.x = uMath_Cos_Deg(angle) * radius;
		vertices[i].point.y = uMath_Sin_Deg(angle) * radius;
		vertices[i].point.z = 0.0;
		vertices[i].attributeSet = NULL;
		
		color.r = 1;
		color.g = 0;
		color.b = 1; 
		vertices[i].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeDiffuseColor, &color);

		meshVertices[i] = Q3Mesh_VertexNew(geometry, &vertices[i]);		
	}

	meshFace = Q3Mesh_FaceNew(geometry, kNumVertices, meshVertices, NULL);

	return geometry;
}



McoStatus GamutWin::AddVertex(int i,double *lab,TQ3Vertex3D *vertices,TQ3MeshVertex *meshVertices,TQ3GeometryObject geometryObject)
{
McoStatus status;
TQ3ColorRGB 		color  = { 0.0, 0.0, 0.0 };
TQ3Error			qd3_error;

	vertices[i].point.x = lab[1]/256.0; 
	vertices[i].point.y = lab[2]/256.0; 
	vertices[i].point.z = 0.5; 		
	
	labtonxyzinplace(lab,1);

	if (xyztorgb == NULL) goto bail;
	status = xyztorgb->compute(lab, monitor_z, 1);
	if(status != MCO_SUCCESS)
		goto bail;	

	color.r = lab[0]/256;
	color.g = lab[1]/256;
	color.b = lab[2]/256; 
	vertices[i].attributeSet = Q3AttributeSet_New();
	Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeDiffuseColor, &color);
	
	qd3_error = Q3Error_Get(NULL);
	if (qd3_error != kQ3ErrorNone) goto bail;
	
	meshVertices[i] = Q3Mesh_VertexNew(geometryObject, &vertices[i]);		
		
return MCO_SUCCESS;

bail:
return MCO_FAILURE;
}



TQ3GeometryObject	GamutWin::NewGammutObject(long start, long end,McoHandle gammutSurfaceH)
{
	TQ3GeometryObject	geometryObject = NULL;
	TQ3GroupObject		group;
	TQ3Vertex3D			*vertices = NULL;
	TQ3Vertex3D			norms;
	TQ3MeshVertex		*meshVertices = NULL;
	TQ3MeshVertex		tempmeshVertices[4];
	TQ3MeshFace			meshFace;
	
	long				c,i,j,k,k2,m;
	float				uuValue,
						uuMin, uuMax, uuStep,
						radius;
	TQ3Boolean			hasUVAttributes;
	TQ3Param2D			param2D;
	
	double 				*gammutSurface;
	double				lab[4];
	McoStatus			status;
	double				x,y,z,X,Y,Z;
	TQ3Error			qd3_error;
	int in,in2;
	
	qd3_error = Q3Error_Get(NULL);

	gammutSurface = (double*)McoLockHandle(gammutSurfaceH);
	
	double Lslice = 50.0;
		 
	uuMin =    0.0;
	uuMax =  360.0;
	uuStep =   9.0;

		
	// Create a mesh 
	geometryObject = Q3Mesh_New();
	if (geometryObject == NULL) return NULL;


	vertices = (TQ3Vertex3D *) NewPtr (500 * sizeof(TQ3Vertex3D));
	if (vertices == NULL)	return NULL;
	
	meshVertices = (TQ3MeshVertex*) NewPtr (500 * sizeof(TQ3MeshVertex));
	

	i = 0;
	j = (int)(Lslice*2.56);
	
	// build all of the vertices
	lab[0] = 50;
	lab[1] = 0;
	lab[2] = 0;
	
	AddVertex(i,lab,vertices,meshVertices,geometryObject);
	i++;
				
	for (in = 2; in <4; in++)
		{
		for (uuValue = uuMax; uuValue > uuMin; uuValue -= uuStep)
			{
			k2 = uuValue;		
			if (k2 >= 360) k2 = k2-360;
					
			lab[0] = gammutSurface[j*4+k2*4*256];
			lab[1] = (in*gammutSurface[j*4+k2*4*256+1])/3.0;
			lab[2] = (in*gammutSurface[j*4+k2*4*256+2])/3.0;
					
			AddVertex(i,lab,vertices,meshVertices,geometryObject);				
			i++;	

			}
		}


	// build the faces
	for (in = 2; in <4; in++)
		{
		c = 1;
		for (uuValue = uuMax; uuValue > uuMin; uuValue -= uuStep)
			{
			i = 0;
			if (in == 2)
				{
				tempmeshVertices[i++] = meshVertices[0];
				}
			else 
				{
				if (c==uuMax/uuStep) m = 1;
				else m = c+1;
				tempmeshVertices[i++] = meshVertices[m];
				m = c;
				tempmeshVertices[i++] = meshVertices[m];	
				}
			
			m = c+(in-2)*uuMax/uuStep;
			tempmeshVertices[i++] = meshVertices[m];
			if (c==uuMax/uuStep) m = 1+(in-2)*uuMax/uuStep;
			else m = 1+c+(in-2)*uuMax/uuStep;	 
			tempmeshVertices[i++] = meshVertices[m];
			c++;
			meshFace = Q3Mesh_FaceNew(geometryObject, i, tempmeshVertices, NULL);
			}
		}	
	

	DisposePtr ((char *) vertices);
	vertices = NULL;
	
	DisposePtr ((char *) meshVertices);
	meshVertices = NULL;
	
	McoUnlockHandle(gammutSurfaceH);	

	return geometryObject;
		
bail:
	if (vertices != NULL) DisposePtr ((char *) vertices);
	if (meshVertices != NULL) DisposePtr ((char *) meshVertices);
	McoUnlockHandle(gammutSurfaceH);
	return NULL;
} 


TQ3GeometryObject	GamutWin::NewGammutObject(long start, long end,McoHandle gammutSurfaceH)
{
	TQ3GeometryObject	geometryObject = NULL;
	TQ3GroupObject		group;
	TQ3Vertex3D			*vertices = NULL;
	TQ3Vertex3D			norms;
	TQ3MeshVertex		*meshVertices = NULL;
	TQ3MeshFace			meshFace;
	
	long				i,j,k,k2;
	float				uuValue, vvValue,
						uuMin, uuMax, uuStep,
						vvMin, vvMax, vvStep,
						radius;
	TQ3Boolean			hasUVAttributes;
	TQ3Param2D			param2D;
	
	double 				*gammutSurface;
	double				lab[4];
	McoStatus			status;
	double				x,y,z,X,Y,Z;
	TQ3Error			qd3_error;
	int in,in2;
	
	qd3_error = Q3Error_Get(NULL);

	gammutSurface = (double*)McoLockHandle(gammutSurfaceH);
	
	double Lslice = 128.0;
		 
	uuMin =    0.0;
	uuMax =  360.0;
	vvMin = 	0.0;
	vvMax =   0.0;
	uuStep =   9.0;
	vvStep =   9.0;	 

		
	// Create a mesh 
	geometryObject = Q3Mesh_New();
	if (geometryObject == NULL) return NULL;


	vertices = (TQ3Vertex3D *) NewPtr (500 * sizeof(TQ3Vertex3D));
	if (vertices == NULL)	return NULL;
	
	meshVertices = (TQ3MeshVertex*) NewPtr (500 * sizeof(TQ3MeshVertex));
	
	
	vvValue = Lslice;
	
	j = (vvValue*1.416+127);
	
	for (in = 2; in <4; in++)
		{
	
		for (uuValue = uuMax; uuValue >= uuMin; uuValue -= uuStep)
			{
			i = 0;	
			if (in == 2)
				{
					
				lab[0] = 50;
				lab[1] = 0;
				lab[2] = 0;
				
				AddVertex(i,lab,vertices,meshVertices,geometryObject);
				i++;
				}
			
			else 
				{
				in2 = in-1;
				k2 = uuValue-uuStep;
				if (k2 >= 360) k2 = k2-360;
				if (k2 < 0) k2 = 360+k2;
			
				lab[0] = gammutSurface[j*4+k2*4*256];
				lab[1] = (in2*gammutSurface[j*4+k2*4*256+1])/3.0;
				lab[2] = (in2*gammutSurface[j*4+k2*4*256+2])/3.0;
					
				AddVertex(i,lab,vertices,meshVertices,geometryObject);				
				i++;

				k2 = uuValue;
				if (k2 >= 360) k2 = k2-360;
			
				lab[0] = gammutSurface[j*4+k2*4*256];
				lab[1] = (in2*gammutSurface[j*4+k2*4*256+1])/3.0;
				lab[2] = (in2*gammutSurface[j*4+k2*4*256+2])/3.0;
					
				AddVertex(i,lab,vertices,meshVertices,geometryObject);				
				i++;

				}
					
			k2 = uuValue;
			if (k2 >= 360) k2 = k2-360;		
						
			lab[0] = gammutSurface[j*4+k2*4*256];
			lab[1] = (in*gammutSurface[j*4+k2*4*256+1])/3.0;
			lab[2] = (in*gammutSurface[j*4+k2*4*256+2])/3.0;
					
			AddVertex(i,lab,vertices,meshVertices,geometryObject);				
			i++;	

			k2 = uuValue-uuStep;
			if (k2 >= 360) k2 = k2-360;
			if (k2 < 0 ) k2 = 360+k2;
			
			lab[0] = gammutSurface[j*4+k2*4*256];
			lab[1] = (in*gammutSurface[j*4+k2*4*256+1])/3.0;
			lab[2] = (in*gammutSurface[j*4+k2*4*256+2])/3.0;
					
			AddVertex(i,lab,vertices,meshVertices,geometryObject);			
			i++;

			
			meshFace = Q3Mesh_FaceNew(geometryObject, i, meshVertices, NULL);
			}
		}

	DisposePtr ((char *) vertices);
	vertices = NULL;
	
	DisposePtr ((char *) meshVertices);
	meshVertices = NULL;
	
	McoUnlockHandle(gammutSurfaceH);	

	return geometryObject;
		
bail:
	if (vertices != NULL) DisposePtr ((char *) vertices);
	if (meshVertices != NULL) DisposePtr ((char *) meshVertices);
	McoUnlockHandle(gammutSurfaceH);
	return NULL;
} 
*/


// Build a gammut object


TQ3GeometryObject	GamutWin::NewGammutObject(long start, long end,McoHandle gammutSurfaceH)
{
	TQ3GeometryObject	geometryObject = NULL;
	TQ3TriGridData		data;
	TQ3Vertex3D			*vertices = NULL;
	TQ3Vertex3D			norms;
	unsigned long		i,j,k;
	float				uuValue, vvValue,
						uuMin, uuMax, uuStep,
						vvMin, vvMax, vvStep,
						radius;
	TQ3Boolean			hasUVAttributes;
	TQ3Param2D			param2D;
	TQ3ColorRGB 		color  = { 0.0, 0.0, 0.0 };
	double 				*gammutSurface;
	double				lab[4];
	McoStatus			status;
	double				x,y,z,X,Y,Z;
	TQ3Error			qd3_error;

	qd3_error = Q3Error_Get(NULL);
//	do {
//		qd3_error = Q3Error_Get(NULL);
//		} while (qd3_error != kQ3ErrorNone);

	gammutSurface = (double*)McoLockHandle(gammutSurfaceH);
	
	// uuValue is an angle used to revolve the cross section about the y axis
	// vvValue is an angle used to generate each half circular cross section
	 
	uuMin =    0.0;
	uuMax =  360.0;
	vvMin = 	-99.0;
	vvMax =   99.0;
	uuStep =   9.0;
	vvStep =   9.0;
		
	radius = 1.0;

	// Setup TQ3TriGridData 
	data.numRows		= (unsigned long) ((vvMax - vvMin) / vvStep) + 1;
	data.numColumns		= (unsigned long) ((uuMax - uuMin) / uuStep) + 1;
	data.facetAttributeSet	= NULL;
	data.triGridAttributeSet= NULL;

	data.vertices = (TQ3Vertex3D *) NewPtr (data.numRows * data.numColumns * sizeof(TQ3Vertex3D));
	if (data.vertices == NULL)	return NULL;

	// Set trigrid vertices and shading UVs, if it hasUVAttributes 
	vertices = data.vertices;
	i = 0;
	for (vvValue = vvMin; vvValue <= vvMax; vvValue += vvStep)
	{
		for (uuValue = uuMax; uuValue >= uuMin; uuValue -= uuStep)
		{
		if ((vvValue>vvMin) && (vvValue<vvMax))
			{
			j = (vvValue*1.416+127);
			k = uuValue;
			if (k >= 360) k = 0;
			radius = gammutSurface[j*4+k*4*256+3];
			}
		else if (vvValue==vvMin)
			{
			j = ((vvValue+vvStep)*1.416+127);
			k = 0;
			if (k >= 360) k = 0;
			radius = 0;
			}
		else 
			{
			j = ((vvValue-vvStep)*1.416+127);
			k = 0;
			if (k >= 360) k = 0;
			radius = 0;
			}
		//Add the specified color attribute to the attribute set.
		lab[0] = gammutSurface[j*4+k*4*256];
		lab[1] = gammutSurface[j*4+k*4*256+1];
		lab[2] = gammutSurface[j*4+k*4*256+2];
		
		labtonxyzinplace(lab,1);
		
		X = lab[0]*96.442;
		Y = lab[1]*100;
		Z = lab[2]*82.521;
		
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
		

		vertices[i].point.x = x; 
		vertices[i].point.y = y; 
		vertices[i].point.z = gammutSurface[j*4+k*4*256]/100; 		


		if (xyztorgb == NULL) goto bail;
		status = xyztorgb->compute(lab, monitor_z, 1);
		if(status != MCO_SUCCESS)
			goto bail;	

		color.r = lab[0]/256;
		color.g = lab[1]/256;
		color.b = lab[2]/256;
		vertices[i].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeDiffuseColor, &color);
		
		qd3_error = Q3Error_Get(NULL);
		if (qd3_error != kQ3ErrorNone) goto bail;
		
		norms.point.x = uMath_Cos_Deg(vvValue) * uMath_Cos_Deg(uuValue) * (radius+2);
		norms.point.y = uMath_Sin_Deg(vvValue)*129;
		norms.point.z = uMath_Cos_Deg(vvValue) * uMath_Sin_Deg(uuValue) * (radius+2);
		
		Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeNormal, &norms);

		qd3_error = Q3Error_Get(NULL);
		if (qd3_error != kQ3ErrorNone) goto bail;

		i++;
		}
	}

	geometryObject = Q3TriGrid_New (&data);

	qd3_error = Q3Error_Get(NULL);
	if (qd3_error != kQ3ErrorNone) goto bail;
	
	for (i = 0; i < data.numRows * data.numColumns; i++)
		Q3Object_Dispose(vertices[i].attributeSet);

	DisposePtr ((char *) data.vertices);
	vertices = NULL;
	
	McoUnlockHandle(gammutSurfaceH);	
		
	return	geometryObject;
bail:
	if (vertices != NULL) DisposePtr ((char *) vertices);
	McoUnlockHandle(gammutSurfaceH);
	return NULL;
} 

double CIELabRef[] = {
	 -128,-128 , 128,-128, 128,128 , -128,128 };

#define NumCIELabRef  (4)
	 
double CIELuvRef[] = {
//	0.1694, 0.5025,
	 0.26, 0.02,
	 0.18, 0.085,
	 0.09, 0.25,
	 0.05, 0.35,
	 0.005, 0.5,
	 0.00, 0.55,
	 0.02, 0.58,
	 0.05, 0.59,
	 0.10, 0.58,
	 0.33, 0.55,
	 0.625, 0.50,
	 0.45, 0.275
	};
	
#define NumCIELuvRef  (12)	
	
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

#define NumCIExyzRef  (40)

#define bigger(a, b) ((a >= b) ? a :b)
#define smaller(a, b) ((a < b) ? a :b)

// build a polygon that is a CIELab refrence
TQ3GeometryObject GamutWin::NewPolyGCIELabRef(void)
{
	TQ3GeometryObject	geometryObject = NULL;
	//TQ3TriGridData		ciepoly;
	TQ3PolygonData ciepoly;
	TQ3Vertex3D			*vertices = NULL;
	long i;
	double	Lxy[4];
	double	lab[4];
	McoStatus	state;
	TQ3ColorRGB 		color  = { 0.0, 0.0, 0.0 };
	double big_ab,big_rgb;


ciepoly.numVertices = NumCIExyzRef;
ciepoly.vertices = (TQ3Vertex3D *) NewPtr (NumCIExyzRef * sizeof(TQ3Vertex3D));
	if (ciepoly.vertices == NULL)	return NULL;
	
vertices = 	ciepoly.vertices;
	
ciepoly.polygonAttributeSet = NULL;
for (i=0; i<NumCIExyzRef; i++)
	{
	Lxy[0] = 50;
	Lxy[1] = CIExyzRef[i*4+1];
	Lxy[2] = CIExyzRef[i*4+2];
	
	vertices[i].point.x = Lxy[1];
	vertices[i].point.y = Lxy[2];
	vertices[i].point.z = 0.5;
	
	sxyztolab(Lxy,lab);
	
	big_ab = bigger(fabs(lab[1]),fabs(lab[2]));
	if (big_ab>127)
		{
		lab[1] = lab[1]*127/big_ab;
		lab[2] = lab[2]*127/big_ab;
		}
		
	labtonxyzinplace(lab,1);
	if (xyztorgb == NULL) return NULL;
	state = xyztorgb->compute(lab, monitor_z, 1);
	if(state != MCO_SUCCESS)
		return NULL;	

	
	big_rgb = bigger(lab[1],bigger(lab[2],lab[3]));
	if (big_rgb>255)
		{
		lab[0] = lab[0]*255/big_rgb;
		lab[1] = lab[1]*255/big_rgb;
		lab[2] = lab[2]*255/big_rgb;
		}
	color.r = lab[0]/256;
	color.g = lab[1]/256;
	color.b = lab[2]/256;
	vertices[i].attributeSet = Q3AttributeSet_New();
	Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeDiffuseColor, &color);
	if (Q3Error_Get(NULL) != kQ3ErrorNone) goto bail;
	}
	
geometryObject = Q3Polygon_New(&ciepoly);
if (Q3Error_Get(NULL) != kQ3ErrorNone) goto bail;

for (i = 0; i < NumCIExyzRef; i++)
	Q3Object_Dispose(vertices[i].attributeSet);	
	
DisposePtr ((char *) vertices);
return	geometryObject;
bail:
if (vertices != NULL) DisposePtr ((char *) vertices);
McoUnlockHandle(gammutSurfaceH);
return NULL;
}



// build a poly-line that is a CIELab refrence
TQ3GeometryObject GamutWin::NewPolyLCIELabRef(void)
{
TQ3GeometryObject	geometryObject = NULL;
TQ3PolyLineData ciepoly;
TQ3Vertex3D			*vertices;
long i;

ciepoly.numVertices = NumCIELabRef;
ciepoly.vertices = (TQ3Vertex3D *) NewPtr (NumCIELabRef * sizeof(TQ3Vertex3D));
	if (ciepoly.vertices == NULL)	return NULL;
	
vertices = 	ciepoly.vertices;
	
ciepoly.segmentAttributeSet = NULL;
ciepoly.polyLineAttributeSet = NULL;
for (i=0; i<NumCIELabRef; i++)
	{
	vertices[i].point.x = CIELabRef[i*2];
	vertices[i].point.y = 0;
	vertices[i].point.z = CIELabRef[i*2];

	}
geometryObject = Q3PolyLine_New(&ciepoly);
if (Q3Error_Get(NULL) != kQ3ErrorNone) goto bail;
	
DisposePtr ((char *) vertices);
return	geometryObject;

bail:

if (vertices != NULL) DisposePtr ((char *) vertices);
return NULL;
}





/*unsigned long numVertices;
TQ3Vertex3D *vertices;
TQ3AttributeSet *segmentAttributeSet;
TQ3AttributeSet polyLineAttributeSet; */

// build a line set that is the xy axies
TQ3GeometryObject GamutWin::NewLxyAx(void)
{
TQ3GeometryObject	geometryObject = NULL;
TQ3PolyLineData 		linedata;
TQ3Vertex3D			*vertices = NULL;
long i;
TQ3GroupObject 		group;	


group = Q3DisplayGroup_New ();

linedata.numVertices = 3;
linedata.vertices = (TQ3Vertex3D *) NewPtr (3 * sizeof(TQ3Vertex3D));
	if (linedata.vertices == NULL)	return NULL;

vertices = 	linedata.vertices;
	
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

geometryObject = Q3PolyLine_New(&linedata);
Q3Group_AddObject (group, geometryObject);
Q3Object_Dispose(geometryObject);	

if (Q3Error_Get(NULL) != kQ3ErrorNone) goto bail;

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
Q3Group_AddObject (group, geometryObject);
Q3Object_Dispose(geometryObject);	
DisposePtr ((char *) linedata.vertices);
vertices = NULL;
if (Q3Error_Get(NULL) != kQ3ErrorNone) goto bail;

geometryObject = MarkerFromText("\px",1,0,0);
Q3Group_AddObject (group, geometryObject);
Q3Object_Dispose(geometryObject);	
if (Q3Error_Get(NULL) != kQ3ErrorNone) goto bail;

geometryObject = MarkerFromText("\py",0,1,0);
Q3Group_AddObject (group, geometryObject);
Q3Object_Dispose(geometryObject);	
if (Q3Error_Get(NULL) != kQ3ErrorNone) goto bail;

geometryObject = MarkerFromText("\pL",0,0,1);
Q3Group_AddObject (group, geometryObject);
Q3Object_Dispose(geometryObject);	
if (Q3Error_Get(NULL) != kQ3ErrorNone) goto bail;

return	group;

bail:

if (vertices != NULL) DisposePtr ((char *) vertices);
return NULL;
}


//-------------------------------------------------------------------------------------------
// change the geometry of the object type with number num
McoStatus GamutWin::ChangeGeometry (McoQ3DType type, long num)
{
	TQ3Status			status;
	TQ3Object			object = NULL;
	TQ3GroupPosition	position,last_position = 0L;
	unsigned long		triGridLibNum;
	McoQ3DObject 		*gammutObj;

	if (Document.fModel == nil)
		return MCO_OBJECT_NOT_INITIALIZED;

	status = Q3Group_GetFirstPositionOfType(Document.fModel, kQ3ShapeTypeGeometry, &position);
	while (status == kQ3Success  &&  position != nil)
		{
		Q3Group_GetPositionObject(Document.fModel,position,&object);
		gammutObj = getObject(object);
		if (gammutObj) if (gammutObj->IsTypeNum(type,num))
			{
			object = Q3Group_RemovePosition(Document.fModel, position);
			Q3Object_Dispose(object);
			removeObject(object);
			object = NULL;
			break;
			}
		last_position = position;
		Q3Group_GetNextPositionOfType(Document.fModel, kQ3ShapeTypeGeometry, &position);
		}

	switch (type) {
		case MP_GammutSurface:
			object = NewGammutObject(start,end,gammutSurfaceH);
			break;
		case MP_CIELabRef_PG:
			object = NewPolyGCIELabRef();
			break;
		case MP_CIELabRef_PL:
			object = NewGammutObject(start,end,gammutSurfaceH);
			break;
		case MP_LxyAx:
			object = NewLxyAx();
			break;
			}
			
	if (object == NULL)
		return MCO_QD3D_ERROR;
	addObject(object,type,num);
	if (last_position != nil) Q3Group_AddObjectBefore (Document.fModel,last_position, object);
	else Q3Group_AddObject (Document.fModel, object);

	Q3Object_Dispose(object);
	return MCO_SUCCESS;
}



McoStatus GamutWin::showLabValue(TQ3Point3D *xyzpoint)
{
short	iType;
Handle	iHandle;
Rect	iRect;
Str255	st;
double 	Lxy[3],Lab[3];
double	big_rgb;
McoStatus	state = MCO_SUCCESS;

	// convert to Lxy
	Lxy[0] = xyzpoint->z*100;
	Lxy[1] = xyzpoint->x;
	Lxy[2] = xyzpoint->y;
	
	sxyztolab(Lxy,Lab);

	GetDItem (dp, ids[GMW_L_text], &iType, &iHandle, &iRect);
	NumToString((int32)Lab[0],st);
	SetIText(iHandle,st);
	
	GetDItem (dp, ids[GMW_a_text], &iType, &iHandle, &iRect);
	NumToString((int32)Lab[1],st);
	SetIText(iHandle,st);


	GetDItem (dp, ids[GMW_b_text], &iType, &iHandle, &iRect);
	NumToString((int32)Lab[2],st);
	SetIText(iHandle,st);

	// convert to rgb
	
	labtonxyzinplace(Lab,1);
	if (xyztorgb == NULL) return MCO_OBJECT_NOT_INITIALIZED;
	state = xyztorgb->compute(Lab, monitor_z, 1);
	if(state != MCO_SUCCESS)
		return state;	

	
	big_rgb = bigger(Lab[1],bigger(Lab[2],Lab[3]));
	if (big_rgb>255)
		{
		Lab[0] = Lab[0]*255/big_rgb;
		Lab[1] = Lab[1]*255/big_rgb;
		Lab[2] = Lab[2]*255/big_rgb;
		}
		
	// draw box
	DrawColorBox(ids[GMW_color_box],Lab);
	
	
	return MCO_SUCCESS;
}



McoStatus GamutWin::get3Dinfo(Point clickPoint)
{

	TQ3WindowPointPickData			myWPPickData;
	TQ3PickObject						myPickObject;
	unsigned long						myNumHits;
	unsigned long						myIndex;
	Point								myPoint;
	TQ3Point2D							my2DPoint;
	TQ3ViewObject						myView;
	long 								state;
	TQ3Point3D							point,LPoint;
	TQ3PickDetail						validMask;
	TQ3Matrix4x4 						LtoG;

	// Get the window coordinates of a mouse click.
	SetPort(dp);
	myPoint = clickPoint;				// get location of mouse click
	//GlobalToLocal(&myPoint);				// convert to window coordinates
	my2DPoint.x = myPoint.h; // - box_rect.left;				// configure a 2D point
	my2DPoint.y = myPoint.v; // - box_rect.top;

	// Set up picking data structures.
	// Set sorting type: objects nearer to pick origin are returned first.
	myWPPickData.data.sort = kQ3PickSortNearToFar; //kQ3PickSortNone
	myWPPickData.data.mask = kQ3PickDetailMaskPickID | kQ3PickDetailMaskXYZ | 
			 				 kQ3PickDetailMaskObject | kQ3PickDetailMaskLocalToWorldMatrix;
	myWPPickData.data.numHitsToReturn = 1; // kQ3ReturnAllHits;
	myWPPickData.point = my2DPoint;
	myWPPickData.vertexTolerance = 1.0;
	myWPPickData.edgeTolerance = 1.0;

	// Create a new window-point pick object.
	myPickObject = Q3WindowPointPick_New(&myWPPickData);

	myView = Document.fView;

	// Pick a group object.
	Q3View_StartPicking(myView, myPickObject);
	do {
		SubmitScene(&Document);
	} while (Q3View_EndPicking(myView) == kQ3ViewStatusRetraverse);

	// See whether any hits occurred.
	state = Q3Pick_GetNumHits(myPickObject, &myNumHits);
	if ((state != kQ3Success) || (myNumHits == 0)) {
		Q3Object_Dispose(myPickObject);
		return MCO_SUCCESS;
	}

	// Process each hit.
	for (myIndex = 0; myIndex < myNumHits; myIndex++) {
		Q3Pick_GetPickDetailValidMask(myPickObject, myIndex, &validMask);

		if (!(validMask & kQ3PickDetailMaskObject)  ||
		    ! (validMask & kQ3PickDetailMaskXYZ) ||
		    ! (validMask & kQ3PickDetailMaskLocalToWorldMatrix)) goto bail;

	
		Q3Pick_GetPickDetailData(myPickObject, myIndex, kQ3PickDetailMaskLocalToWorldMatrix, &LtoG);
		Q3Pick_GetPickDetailData(myPickObject, myIndex, kQ3PickDetailMaskXYZ, &point);

		// operate on myHitData, then...
		//SysBeep(10);	
		translateGlobalToLocal(&LtoG,&point,&LPoint);
		showLabValue(&LPoint);
	}

	// Dispose of all hits in the hit list.
	Q3Pick_EmptyHitList(myPickObject);

	// Dispose of the pick object.
	Q3Object_Dispose(myPickObject);

bail:
	return MCO_SUCCESS;
}


// Get the active region
McoStatus GamutWin::makeEventRegion(RgnHandle previewRgn,RgnHandle mouseRgn)
{
Rect r;
Handle h;
short itemType;

GetDItem (dp,ids[QD3D_Box], &itemType, &h, &r);

RectRgn(previewRgn,&r);
XorRgn(mouseRgn,previewRgn,mouseRgn);

return MCO_SUCCESS;
}


McoStatus GamutWin::doPointInWindowActiveRgn(Point where,WinEve_Cursors &cursortype)
{
McoStatus state = MCO_SUCCESS;
unsigned char 	*previewOutBuf;
unsigned char 	*previewInBuf;
GrafPtr			oldGraf;
Rect r1,r2;
Handle h;
short itemType;
double  labc[3];
double	lab[3];
double  rgb[3];

WindowPtr	oldP;
	
GetPort(&oldP);
SetPort( dp ) ;

GlobalToLocal(&where);

GetDItem (dp,ids[QD3D_Box], &itemType, &h, &r1);

if (PtInRect(where,&r1))
	{
	cursortype = WC_CrossTemp;
	get3Dinfo(where);
	}
else 
	{
	cursortype = WC_SetBack;
	}
SetPort(oldP);
return state;
} 