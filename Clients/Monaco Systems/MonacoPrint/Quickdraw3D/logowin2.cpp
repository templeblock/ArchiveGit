/*********************************************/
/* logowin2.c							 	 */
/* Display 3D gammut surface in window		 */
/* Jan 19, 1996							     */
/*********************************************/
/*   James Vogh 							 */
/*********************************************/

#include <QD3DGeometry.h>
#include "logowin2.h"
#include "mcostat.h"

#include <Memory.h>
#include "math.h"
#include "colortran.h"
#include "mcofile.h"

#include "AttributeSet_Lib.h"

#include "MathUtilities.h"

#include "Textures2.h"


// initialize the 3D and load an object
LogoWin2::LogoWin2(XyztoRgb *xtor,double mg) 
{
	TQ3Status	myStatus;
	//Rect		rBounds;
	FSSpec		theFileSpec ;				// the file we are opening
	Str255 		title = "\p";
	McoStatus   status;
	Handle			item_handle;
	short			item_type;
	int			Q3DIDS[] = LW_IDS;
	int			i;
	
	priority = 3;
	
	xyztorgb = xtor;
	monitor_z = mg;
	
	ambient = 0.6;
	point = 1.5;
	fill = 0.2;
	
	Document.fCameraFrom.x = 0.0; 
	Document.fCameraFrom.y = 0.0;
	Document.fCameraFrom.z = 30.0;
	
	Document.fCameraTo.x = 0.0; 
	Document.fCameraTo.y = 0.0;
	Document.fCameraTo.z = 0.0;
	
	WinNum = 0;
	WinType = Startup_Dialog;
	
//	the_window = NewCWindow(nil,&rBounds,title,false,noGrowDocProc,(WindowPtr)-1,true,nil);
//	if (the_window == NULL) goto fail;

	for (i=0; i<NUM_QD3D_IDS; i++) ids[i] = Q3DIDS[i];

	setDialog(ids[QD3D_Dial],0,-140);
	the_window = dp;

	GetDItem (dp, ids[QD3D_Box], &item_type, &item_handle, &box_rect);

	// initialise our document structure
	InitDocumentData( &Document ) ;
	
	num_objects = 0;
	
	status = ChangeGeometry(MP_Logo	,0);
	if (status != MCO_SUCCESS) goto fail;
			
	
	AdjustCamera(	&Document,
					(box_rect.right - box_rect.left),
					(box_rect.bottom - box_rect.top) ) ;

	SetWTitle( the_window, "\p" );
	ShowWindow( the_window );
	SetPort( the_window );

	DocumentDraw3DData(&Document);
	DocumentDrawOnscreen( &Document, &box_rect ) ;
	
	Current_Button = ids[QD3D_Rotate];
	Disable(dp,ids[QD3D_Rotate]);
	
	tick_start = TickCount();
	
fail:	
	return;

	//delete this;
}

LogoWin2::~LogoWin2(void) 
{
	TQ3Status	myStatus;

	
	DisposeDocumentData( &Document ) ;

	//	Close our connection to the QuickDraw 3D library
	//CloseDialog(dialogptr);
	//if (the_window != NULL) DisposeWindow(the_window);
}		
		
		
// Build a logo object


TQ3GeometryObject	LogoWin2::NewLogoObject(void)
{
	TQ3GeometryObject	geometryObject = NULL;
	TQ3TriGridData		data;
	TQ3Vertex3D			*vertices;
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

	
	// uuValue is an angle used to revolve the cross section about the y axis
	// vvValue is an angle used to generate each half circular cross section

	uuMin =    0.0;
	uuMax =  360.0;
	vvMin = - 90.0;
	vvMax =   90.0;
		
	uuStep =  12.0;
	vvStep =  12.0;
		
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
		vertices[i].point.x = uMath_Cos_Deg(vvValue) * uMath_Cos_Deg(uuValue) * radius;
		vertices[i].point.y = 1.5*uMath_Sin_Deg(vvValue) * radius;
		vertices[i].point.z = uMath_Cos_Deg(vvValue) * uMath_Sin_Deg(uuValue) * radius;	
		
		param2D.u = 1.0 - ((uuValue - uuMin) / (uuMax - uuMin));
		param2D.v = (vvValue - vvMin) / (vvMax - vvMin);
								
		vertices[i].attributeSet = Q3AttributeSet_New();
		Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeShadingUV, &param2D);

		norms.point.x = uMath_Cos_Deg(vvValue) * uMath_Cos_Deg(uuValue) * (radius+1);
		norms.point.y = 1.5*uMath_Sin_Deg(vvValue)*(radius+1);
		norms.point.z = uMath_Cos_Deg(vvValue) * uMath_Sin_Deg(uuValue) * (radius+1);
		
		Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeNormal, &norms);

		i++;
		}
	}

	geometryObject = Q3TriGrid_New (&data);

	for (i = 0; i < data.numRows * data.numColumns; i++)
		Q3Object_Dispose(vertices[i].attributeSet);

	DisposePtr ((char *) vertices);
		
	return	geometryObject;
} 


double CIExyzRef2[] = {	
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
TQ3GeometryObject LogoWin2::NewPolyGCIELabRef(void)
{
	TQ3GeometryObject	geometryObject = NULL;
	//TQ3TriGridData		ciepoly;
	TQ3PolygonData ciepoly;
	TQ3Vertex3D			*vertices;
	long i;
	double	Lxy[4];
	double	lab[4];
	McoStatus	state;
	TQ3ColorRGB 		color  = { 0.0, 0.0, 0.0 };
	double big_ab,big_rgb;
	TQ3Param2D			param2D;

ciepoly.numVertices = NumCIExyzRef;
ciepoly.vertices = (TQ3Vertex3D *) NewPtr (NumCIExyzRef * sizeof(TQ3Vertex3D));
	if (ciepoly.vertices == NULL)	return NULL;
	
vertices = 	ciepoly.vertices;
	
ciepoly.polygonAttributeSet = NULL;
for (i=0; i<NumCIExyzRef; i++)
	{
	Lxy[0] = 50;
	Lxy[1] = CIExyzRef2[i*4+1];
	Lxy[2] = CIExyzRef2[i*4+2];
	
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
	
	param2D.u = Lxy[1]*1.37;
	param2D.v = Lxy[2]*1.1;
								
	Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeShadingUV, &param2D);

	}
	
geometryObject = Q3Polygon_New(&ciepoly);

for (i = 0; i < NumCIExyzRef; i++)
	Q3Object_Dispose(vertices[i].attributeSet);	
	
DisposePtr ((char *) vertices);
return	geometryObject;
}


//-------------------------------------------------------------------------------------------
// change the geometry of the object type with number num
McoStatus LogoWin2::ChangeGeometry (McoQ3DType type, long num)
{
	TQ3Status			status;
	TQ3Object			object = NULL;
	TQ3GroupPosition	position;
	unsigned long		triGridLibNum;
	McoQ3DObject 		*gammutObj;

	if (Document.fModel == nil)
		return MCO_OBJECT_NOT_INITIALIZED;

	status = Q3Group_GetFirstPositionOfType(Document.fModel, kQ3ShapeTypeGeometry, &position);
	while (status == kQ3Success  &&  position != nil)
		{
		gammutObj = getObject(object);
		if (gammutObj->IsTypeNum(type,num))
			{
			status = Q3Group_GetNextPosition(Document.fModel, &position);
			object = Q3Group_RemovePosition(Document.fModel, position);
			Q3Object_Dispose(object);
			removeObject(object);
			object = NULL;
			break;
			}
		Q3Group_GetNextPositionOfType(Document.fModel, kQ3ShapeTypeGeometry, &position);
		}

	object = NewPolyGCIELabRef();
	//object = NewLogoObject();
			
	if (object == NULL)
		return MCO_FAILURE;
	addObject(object,type,num);
	if (position != nil) Q3Group_AddObjectBefore (Document.fModel, position, object);
	else Q3Group_AddObject (Document.fModel, object);

	AddResourceTextureToGroup( LW_TEXTURE, Document.fModel);

	Q3Object_Dispose(object);
	return MCO_SUCCESS;
}


//-------------------------------------------------------------------------------------------
//
McoStatus LogoWin2::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed)
{
	EventRecord 	event;
	WindowPtr   	window;
	short       	thePart;
	Rect        	screenRect, updateRect;
	Point			aPoint = {100, 100};
	Boolean 		QuitFlag = FALSE;
	McoStatus 		status;
	Window_Events 	code2;

Changed = FALSE;
*numwc = 0;
status = MCO_SUCCESS;

if (TickCount() - tick_start > 100) tick_start = 0;
	if (item == TIMER_ITEM) // close the window
		{
		*numwc = 1;
		wc[0].code = WE_Close_Window;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].doc = 0L;	
		}
	if (item == ids[QD3D_Zoom])
		{
		Enable(dp,Current_Button);
		Disable(dp,ids[QD3D_Zoom]);
		Current_Button = item;
		}
	else if (item == ids[QD3D_Rotate])
		{
		Enable(dp,Current_Button);
		Disable(dp,ids[QD3D_Rotate]);
		Current_Button = item;
		}
	else if (item == ids[QD3D_Translate])
		{
		Enable(dp,Current_Button);
		Disable(dp,ids[QD3D_Translate]);
		Current_Button = item;
		}
	else if (item == ids[QD3D_Info])
		{
		Enable(dp,Current_Button);
		Disable(dp,ids[QD3D_Info]);
		Current_Button = item;
		} 
	else if (item == ids[QD3D_Box])
		{
		if (Current_Button == ids[QD3D_Rotate])
			{
			code2 = WE_Rotate;
			}
		else if (Current_Button == ids[QD3D_Zoom])
			{
			code2 = WE_Scale;
			}
		else if (Current_Button == ids[QD3D_Translate])
			{
			code2 = WE_Translate;
			}
		else if (Current_Button == ids[QD3D_Info])
			{
			status = get3Dinfo(clickPoint);
			}
		ProcessMouseDown(clickPoint,code2);
		}
return status;	
}


McoStatus LogoWin2::updateWindowData(int changed)
{
int i;
double	xrt,yrt,zrt;
//return MCO_SUCCESS;

xrt = 0.00;
yrt = 0.04;
zrt =  0.000;

DoTransform(WE_Rotate,xrt,yrt,zrt);
forceUpdateData();

return MCO_SUCCESS;
}


Boolean LogoWin2::TimerEventPresent(void) 
{
if (TickCount() - tick_start > 2400) return TRUE;
return FALSE;
}



