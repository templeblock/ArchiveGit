//////////////////////////////////////////////////////////////////////////////
// tweaktab.cpp																//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#include <QD3DGeometry.h>
#include "mathutilities.h"
#include "tweaktab.h"
#include "fileformat.h"
#include "errordial.h"
#include "pascal_string.h"
#include "think_bugs.h"
#include "colortran.h"
#include "mcocolsyn.h"

#define ST_DITL_ID		6005
#define ST_DITL_G_ID	6007
#define Light_slider	1
#define Color_slider	2

#define LightViewSlider	21

#define View_Box		20

#define Desired_Box		19
#define Gamut_Box		22
#define Printed_Box		31

#define Desired_View_Box	32
#define Tweak_View_Box	33

#define Tweak_Menu		35
#define Tweak_Name		36
#define New_Tweak		37
#define Delete_Tweak	38

#define Gamut_Tweak_Radio		39
#define Printing_Tweak_Radio	40

#define Zoom_Button		41
#define Move_Button		42

#define L_text			44

#define Desired_Start	8
#define Gamut_Start		45
#define Gamut_Start_Ne		23
#define Print_Start		11
#define Print_Lab_St	14

#define Desired_View_Text_St 51
#define Tweak_View_Text_St 57
#define View_Text_Type	61

#define Print_Box_Label 4

#define Tweak_Menu_ID	9001

#define Assorted_Names 2007

#define WAIT_PICT	150


/* The PICT ids for the medium slider */
/* disable back, enable unpress back, enable press, disable slide, enable unpress slide, enable press slide */

#define LARGE_PICT_IDS {18013,18012,18012,-32000,18009,18009}

/* The offsets etc. used by the medium sliders */
/* horz offset, horz inset, vert offset, horz text, vert text, horz text, vert text, horz text, vert text, Rect left, rect right, text id */

#define LARGE_OFFSETS {-1,20,0,-32000,-32000,-32000,-32000,-32000,-32000, \
						 1,2,12,13,   86,2,97,13, 1,1, -1}

#define QD_IDS	{-1,20,41,-1,42,-1}

#define NUM_TT_IDS	6

#define CIRCLE_STEP 9

void AdjustCamera2(DocumentPtr			theDocument);
void AdjustCamera2(DocumentPtr			theDocument)
{

	TQ3ViewObject				theView = theDocument->fView ;
	TQ3CameraRange				range;
	TQ3CameraObject				camera;
	float						fov;
		
		
	Q3View_GetCamera( theView, &camera);
	
	Q3ViewAngleAspectCamera_GetFOV(camera, &fov);
	
	fov = fov/2;

	Q3ViewAngleAspectCamera_SetFOV(
		camera, fov);



	Q3Object_Dispose(camera);
	
	return;
}

TweakTab::TweakTab(ProfileDoc *dc,WindowPtr d):WindowTab(d,ST_DITL_ID)
{
	int i;
	int16			large_slid_picts[] = LARGE_PICT_IDS;
	int16			large_slid_off[] = LARGE_OFFSETS;
	Str255			theString,numstr;
	McoColorSync *mon,*lab,*list[2];
	short			tids[] = QD_IDS;
	McoStatus		state = MCO_SUCCESS;
	
	short itemType;
	Handle	h;
	
	idDITL = ST_DITL_ID;


	doc = (ProfileDocInf*)dc;


	refCon1 = 0L;
	refCon2 = 0L;
	lab_rgb_xform = 0L;
	doneGamut = 0;
	startGamut = 0;
	refCon1 = new RefConType;
	refCon2 = new RefConType;
	
	_indexlut = 0L;
	
	setSlider(large_slid_picts,large_slid_off,startNum+Light_slider,refCon1);	
	
	setSlider(large_slid_picts,large_slid_off,startNum+Color_slider,refCon2);

	current_tweak = 0L;

	if (doc->didGammutCompression) 
		{
		doneGamut = 1;
		}

	
	the_window = dp;
	
	for (i=0; i<NUM_TT_IDS; i++) ids[i] = tids[i];


// initialize comminications with the device
	state = doc->openInputDevice(0,1,0);
	if (state != MCO_SUCCESS) McoErrorAlert(state);
// calibrate the device
	if (doc->tcom) doc->tcom->Calibrate();
	
	
	_last_tick_time2 = 0;
	Current_Text = 0;

// set up the parameters

	
	if (doc->printerData->num_tweaks >0)
		{
		current_tweak = doc->printerData->tweaks[0];
		tweak_num = 0;
		}
	else 
		{
		current_tweak = 0L;
		tweak_num = -1;
		}
		
	//CopyParameters();
	
	// set up color conversion
	mon = new McoColorSync;
	mon->setType(Mco_ICC_System);

	lab = new McoColorSync;
	lab->GetProfile(LAB_PROFILE_NAME);
		
	lab_rgb_xform = new McoXformIccMac;
	list[0] = lab;
	list[1] = mon;
	state = lab_rgb_xform->buildCW(list,2);	
	if (state != MCO_SUCCESS) { delete lab_rgb_xform; lab_rgb_xform = 0L;}
		
	delete mon;
	delete lab;
	
	// initialise our document structure
	
	GetDItem (dp, ids[QD3D_Box]+startNum, &itemType, &h, &box_rect);
	
	ambient = 1.0;
	point = 0;
	fill = 0;

	Document.fCameraFrom.x = 0.0; 
	Document.fCameraFrom.y = 0.0;
	Document.fCameraFrom.z = 30.0;

	Document.fCameraTo.x = 0.0; 
	Document.fCameraTo.y = 0.0;
	Document.fCameraTo.z = 0.0;


	SliderL = 50;
	// get the correct 
	if (doc->patchD->type == CMYKPatches)
		gammutSurfaceH = doc->_gammutSurfaceBlackH;
	else 
		gammutSurfaceH = doc->_gammutSurfaceH;
		
	InitDocumentData( &Document ) ;
		
			
	state = ChangeGeometry(MP_TweakAx,0);
	if (state != MCO_SUCCESS) goto fail;

	state = ChangeGeometry(MP_Circle,0);
	if (state != MCO_SUCCESS) goto fail;

	state = ChangeGeometry(MP_Arrow,0);
	if (state != MCO_SUCCESS) goto fail;
	
	state = ChangeGeometry(MP_GamutSlice,0);
	if (state != MCO_SUCCESS) goto fail;
	
	state = ChangeGeometry(MP_BlackBox,0);
	if (state != MCO_SUCCESS) goto fail;

	state = ChangeGeometry(MP_TweakMark1,0);
	if (state != MCO_SUCCESS) goto fail;


	AdjustCamera(	&Document,
					(box_rect.right - box_rect.left),
					(box_rect.bottom - box_rect.top) ) ;	

	AdjustCamera2(&Document);
	
	desiredRGB[0] = 0;
	desiredRGB[1] = 0;
	desiredRGB[2] = 0;
	
	printedRGB[0] = 0;
	printedRGB[1] = 0;
	printedRGB[2] = 0;
	
	gammutRGB[0] = 0;
	gammutRGB[1] = 0;
	gammutRGB[2] = 0;
	
	rgb_d[0] = 0;	
	rgb_d[1] = 0;
	rgb_d[2] = 0;

	rgb_p[0] = 0;	
	rgb_p[1] = 0;
	rgb_p[2] = 0;

	rgb_g[0] = 0;	
	rgb_g[1] = 0;
	rgb_g[2] = 0;
	
	Current_Button = -1;

	if (current_tweak) SliderL = current_tweak->lab_d[0];
	
	updateWindowData(0);
	_updatSliderL();
	_changeTweakData(1);
	_setUpTweakMenu();
	
	validWin = 1;
	return;

fail:
	validWin = 0;
	return;
}




TweakTab::~TweakTab(void)
{
if (lab_rgb_xform) delete lab_rgb_xform;
if (doc != 0L) doc->closeInputDevice();
if (refCon1) delete refCon1;
if (refCon2) delete refCon2;
if (_indexlut) delete _indexlut;
}


void TweakTab::_changeToCalib(void)
{
Str255	st;

ShowDItem(dp,Printed_Box+startNum);
ShowDItem(dp,Print_Box_Label+startNum);
ShowDItem(dp,Print_Start+startNum);
ShowDItem(dp,Print_Start+startNum+1);
ShowDItem(dp,Print_Start+startNum+2);

ShowDItem(dp,Gamut_Start_Ne+startNum);
ShowDItem(dp,Gamut_Start_Ne+startNum+1);
ShowDItem(dp,Gamut_Start_Ne+startNum+2);

ShowDItem(dp,Print_Lab_St+startNum);
ShowDItem(dp,Print_Lab_St+startNum+1);
ShowDItem(dp,Print_Lab_St+startNum+2);

HideDItem(dp,Gamut_Start+startNum);
HideDItem(dp,Gamut_Start+startNum+1);
HideDItem(dp,Gamut_Start+startNum+2);

SetItemText(Tweak_View_Text_St+startNum,"\p   ");
SetItemText(Tweak_View_Text_St+startNum+1,"\p   ");
SetItemText(Tweak_View_Text_St+startNum+2,"\p   ");

GetIndString(st,Assorted_Names,3);

SetItemText(View_Text_Type+startNum,st);
}

void TweakTab::_changeToGamut(void)
{
Str255	st;

HideDItem(dp,Printed_Box+startNum);
HideDItem(dp,Print_Box_Label+startNum);
HideDItem(dp,Print_Start+startNum);
HideDItem(dp,Print_Start+startNum+1);
HideDItem(dp,Print_Start+startNum+2);

HideDItem(dp,Gamut_Start_Ne+startNum);
HideDItem(dp,Gamut_Start_Ne+startNum+1);
HideDItem(dp,Gamut_Start_Ne+startNum+2);

HideDItem(dp,Print_Lab_St+startNum);
HideDItem(dp,Print_Lab_St+startNum+1);
HideDItem(dp,Print_Lab_St+startNum+2);

ShowDItem(dp,Gamut_Start+startNum);
ShowDItem(dp,Gamut_Start+startNum+1);
ShowDItem(dp,Gamut_Start+startNum+2);

SetItemText(Tweak_View_Text_St+startNum,"\p   ");
SetItemText(Tweak_View_Text_St+startNum+1,"\p   ");
SetItemText(Tweak_View_Text_St+startNum+2,"\p   ");

GetIndString(st,Assorted_Names,2);

SetItemText(View_Text_Type+startNum,st);
}


void TweakTab::_setUpTweak(int which)
{
Rect r;
ControlHandle h;
short itemType;

if (!current_tweak) return;

ConvertToRGB(current_tweak->lab_d,desiredRGB);
if ((current_tweak->type == Calibrate_Tweak) || (which == 0)) 
	{
	ConvertToGammutC(current_tweak->lab_d,current_tweak->lab_g);
	}
if (which == 0)
	{
	current_tweak->lab_p[0] = current_tweak->lab_g[0];
	current_tweak->lab_p[1] = current_tweak->lab_g[1];
	current_tweak->lab_p[2] = current_tweak->lab_g[2];
	}
ConvertToRGB(current_tweak->lab_g,gammutRGB);
ConvertToRGB(current_tweak->lab_p,printedRGB);

_copyTextToDial();

GetDItem ( dp, Light_slider+startNum, &itemType, (Handle*)&h, &r );
SetCtlValue (h,(int32)current_tweak->Lr);

GetDItem ( dp, Color_slider+startNum, &itemType, (Handle*)&h, &r );
SetCtlValue ( h,(int32)current_tweak->Cr);

GetDItem ( dp, LightViewSlider+startNum, &itemType, (Handle*)&h, &r );
SetCtlValue ( h,(int32)SliderL);

SetItemTextToNum( L_text+startNum, (int32)SliderL );

		
GetDItem ( dp, Gamut_Tweak_Radio+startNum, &itemType, (Handle*)&h, &r );
SetCtlValue (h, current_tweak->type);
GetDItem ( dp, Printing_Tweak_Radio+startNum, &itemType, (Handle*)&h, &r );
SetCtlValue (h, 1-current_tweak->type);

if (current_tweak->type == Calibrate_Tweak) _changeToCalib();
if (current_tweak->type == GamutComp_Tweak) _changeToGamut();

UpdateWindow();
}

McoStatus TweakTab::_setUpTweaks(void)
{
doc->tweak->Init_Table();
doc->tweak->Modify_Table(doc->printerData->num_tweaks,doc->printerData->tweaks);
// tweak the patches using the tweaking table	

doc->gtweak->Init_Table();
// the gamut compresion tweak
doc->gtweak->Modify_Table(doc->printerData->num_tweaks,doc->printerData->tweaks);
return MCO_SUCCESS;
}

void TweakTab::_changeTweakData(int which)
{
if (!current_tweak) return;

_setUpTweak(which);
if (current_tweak->type == Calibrate_Tweak)
	{
	ModifyCircle(current_tweak->lab_g,current_tweak->Lr,current_tweak->Cr,SliderL);
	ModifyArrow(current_tweak->lab_g,current_tweak->lab_p);
	}
else if (current_tweak->type == GamutComp_Tweak)
	{
	ModifyCircle(current_tweak->lab_d,current_tweak->Lr,current_tweak->Cr,SliderL);
	ModifyArrow(current_tweak->lab_d,current_tweak->lab_g);
	}
	
ModifyCross(current_tweak->lab_d);

UpdateWindow();
}

void TweakTab::_updatSliderL(void)
{
Rect r;
ControlHandle h;
short itemType;

if (!current_tweak) return;

GetDItem ( dp, LightViewSlider+startNum, &itemType, (Handle*)&h, &r );
SetCtlValue ( h,(int32)SliderL);

if (current_tweak->type == Calibrate_Tweak)
	ModifyCircle(current_tweak->lab_g,current_tweak->Lr,current_tweak->Cr,SliderL);
else if (current_tweak->type == GamutComp_Tweak)
	ModifyCircle(current_tweak->lab_d,current_tweak->Lr,current_tweak->Cr,SliderL);
ModifyGammutObject(gammutSurfaceH);
SetItemTextToNum( L_text+startNum, (int32)SliderL );
}

void TweakTab::_setUpTweakMenu(void)
{
	int i;
	MenuHandle menuH;
	short menucount;
	char	str[255];
	Rect r;
	Handle h;
	short itemType;
	ControlHandle	sliderControl;
	

	menuH = GetMenu(Tweak_Menu_ID);
	menucount = CountMItems(menuH);

//delete anything above 1
	for( i = menucount; i > 0; i-- )
			DelMenuItem(menuH,i);	
			
	for (i=0; i<doc->printerData->num_tweaks; i++)
		{
		strcpy(str,doc->printerData->tweaks[i]->name);
		ctopstr(str);
		if (str[0] == 0) copy_str((unsigned char*)str,"\p ");
		InsMenuItem(menuH, (unsigned char*)str, i+1);
		}
		
	if (tweak_num != -1)
		{	
		GetDItem ( dp,Tweak_Menu+startNum, &itemType, (Handle*)&sliderControl, &r );
		SetCtlValue ( sliderControl,tweak_num+1);	
		}
		
	DrawControls(dp);
}


McoStatus TweakTab::CopyParameters(void)
{
if (!doneGamut) return MCO_SUCCESS;
_setUpTweak(0);
_setUpTweakMenu();
return MCO_SUCCESS;
}



	
//-------------------------------------------------------------------------------------------
// change the geometry of the object type with number num
// create a black box behind everything for selecting out of gamut colors
TQ3GeometryObject 	TweakTab::BlackBox(void)
{
TQ3PolygonData	poly;
TQ3GeometryObject object;

TQ3ColorRGB 		color  = { 0.0, 0.0, 0.0 };

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
Q3AttributeSet_Add(poly.polygonAttributeSet, kQ3AttributeTypeDiffuseColor, &color);

object = Q3Polygon_New(&poly);

delete poly.vertices;
return object;
}

McoStatus	TweakTab::ModifyGammutObject(McoHandle gammutSurfaceH)
{
	TQ3GeometryObject	geometryObject = NULL;
	TQ3GroupPosition	position;
	TQ3Status			status;


	// get the object position
	status = Q3Group_GetFirstPositionOfType(Document.fModel, kQ3GeometryTypeMesh, &position);
	if (!position) return MCO_FAILURE;
	// delete it 
	Q3Group_RemovePosition(Document.fModel, position);

	geometryObject = NewGammutObject(gammutSurfaceH);
	
	if (geometryObject == NULL) return MCO_QD3D_ERROR;
	Q3Group_AddObject (Document.fModel, geometryObject);

	Q3Object_Dispose(geometryObject);

	return MCO_SUCCESS;
} 



McoStatus TweakTab::AddVertex(int i,double *lab,TQ3Vertex3D *vertices,TQ3MeshVertex *meshVertices,TQ3GeometryObject geometryObject)
{
McoStatus status;
double			rgb[3];
TQ3ColorRGB 		color  = { 0.0, 0.0, 0.0 };
TQ3Status			qd3_error;

	vertices[i].point.x = lab[1]/128.0; 
	vertices[i].point.y = lab[2]/128.0; 
	vertices[i].point.z = 0.5; 		
	
	ConvertToRGB(lab,rgb);

	color.r = rgb[0]/256;
	color.g = rgb[1]/256;
	color.b = rgb[2]/256; 
	vertices[i].attributeSet = Q3AttributeSet_New();
	qd3_error = Q3AttributeSet_Add(vertices[i].attributeSet, kQ3AttributeTypeDiffuseColor, &color);

	if (qd3_error != kQ3Success) goto bail;
	
	meshVertices[i] = Q3Mesh_VertexNew(geometryObject, &vertices[i]);		
		
return MCO_SUCCESS;

bail:
return MCO_FAILURE;
}

// build a set of luts that is used to find the index into a gamutsurface
McoStatus TweakTab::buildIndexLut(double *gamutSurface)
{
int i,j,k;
double	*gp1,*gp2,*g1,*g2;
double	L;

_indexlut = new short[256*360];
if (_indexlut == 0L) return MCO_MEM_ALLOC_ERROR;

_minL = 101;
_maxL = -1;

for (i=0; i<360; i++)
	{
	gp1 = &gamutSurface[i*256*4];
	gp2 = &gamutSurface[i*256*4+4];
	for (j=0; j<256; j++)
		{
		L = j/2.55;
		g1 = gp1;
		g2 = gp2;
		for (k=0; k<255; k++)
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


inline long TweakTab::getgammutSurfaceIndex(double L,double h)
{
short L_i,h_i;

L_i = (short)(L*2.55);
h_i = (short)h;

return _indexlut[h_i*256+L_i];

return MCO_SUCCESS;
}


TQ3GeometryObject	TweakTab::NewGammutObject(McoHandle gammutSurfaceH)
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
	McoStatus			status = MCO_SUCCESS;
	double				x,y,z,X,Y,Z;
	TQ3Error			qd3_error;
	int in,in2;
	
	TQ3Point2D			point;
	TQ3PolarPoint		ppoint;
	

	gammutSurface = (double*)McoLockHandle(gammutSurfaceH);
	
	if (!_indexlut) status = buildIndexLut(gammutSurface);
	if (status) goto bail;
	
	double Lslice = SliderL;
		 
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
	lab[0] = Lslice;
	lab[1] = 0;
	lab[2] = 0;
	
	status = AddVertex(i,lab,vertices,meshVertices,geometryObject);
	if (status) goto bail;
	i++;
				
	for (in = 2; in <4; in++)
		{
		for (uuValue = uuMax; uuValue > uuMin; uuValue -= uuStep)
			{
			k2 = uuValue;		
			if (k2 >= 360) k2 = k2-360;
				
			if (doc->didGammutCompression) 
				{
				if ((Lslice >= _minL) && (Lslice <= _maxL))
					{	
					j = getgammutSurfaceIndex(Lslice,k2);	
					lab[0] = gammutSurface[j*4+k2*4*256];
					lab[1] = 1.05*(in*gammutSurface[j*4+k2*4*256+1])/3.0;
					lab[2] = 1.05*(in*gammutSurface[j*4+k2*4*256+2])/3.0;
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
				ppoint.r = in*42.6;
				ppoint.theta = 0.01745329252*k2;
				
				Q3PolarPoint_ToPoint2D(&ppoint,&point);

				lab[0] = SliderL;
				lab[1] = point.x;
				lab[2] = point.y;
				}
					
			status = AddVertex(i,lab,vertices,meshVertices,geometryObject);	
			if (status) goto bail;			
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

// build a line set that is the xy axies
TQ3GeometryObject TweakTab::New_ab_ax(void)
{
TQ3GeometryObject	geometryObject = NULL;
TQ3PolyLineData 		linedata;
TQ3Vertex3D			*vertices = NULL;
long i;
TQ3GroupObject 		group;
TQ3Status			error;


group = Q3DisplayGroup_New ();

linedata.numVertices = 2;
linedata.vertices = (TQ3Vertex3D *) NewPtr (3 * sizeof(TQ3Vertex3D));
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
	
DisposePtr ((char *) linedata.vertices);
vertices = NULL;

return	group;

bail:

if (vertices != NULL) DisposePtr ((char *) vertices);
return NULL;
}


McoStatus TweakTab::ModifyCircle(double *lab1,double Lr,double Cr,double currentL)
{
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

if (!validWin) return MCO_FAILURE;

// first get the circle

status = Q3Group_GetFirstPositionOfType(Document.fModel, kQ3GeometryTypePolyLine, &position);

status = Q3Group_GetPositionObject(Document.fModel, position, &object);

Ldist = lab1[0]-currentL;
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


short TweakTab::SetUpCircle(TQ3PolyLineData *linedata,double *lab1,double Lr,double Cr,double currentL)
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

if (Lr == 0) return 0;
if (Cr == 0) return 0;

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

// build a line set that is the xy axies
TQ3GeometryObject TweakTab::NewCircle(double *lab1,double Lr,double Cr,double currentL)
{
TQ3GeometryObject	geometryObject = NULL;
TQ3PolyLineData 	linedata;
TQ3Vertex3D			*vertices = NULL;
long 				i,steps;
TQ3GroupObject 		group;		
TQ3ColorRGB 		color  = { 1.0, 0.0, 0.0 };



steps = 1+360/CIRCLE_STEP;


linedata.numVertices = steps;
linedata.vertices = (TQ3Vertex3D *) NewPtr (steps * sizeof(TQ3Vertex3D));
	if (linedata.vertices == NULL)	return NULL;
	
linedata.segmentAttributeSet = NULL;
linedata.polyLineAttributeSet = NULL;

vertices = 	linedata.vertices;

SetUpCircle(&linedata,lab1,Lr,Cr,currentL);
	

geometryObject = Q3PolyLine_New(&linedata);
if (!geometryObject) return NULL;


DisposePtr ((char *) linedata.vertices);
vertices = NULL;

return	geometryObject;

bail:

if (vertices != NULL) DisposePtr ((char *) vertices);
return NULL;
}

// modify an arrow
McoStatus TweakTab::ModifyArrow(double *lab1,double *lab2)
{
TQ3Status			status;
TQ3GeometryObject	geometryObject = NULL;
TQ3PolyLineData 		linedata;
TQ3Vertex3D			*vertices = NULL;
long i;
TQ3GroupObject 		ggroup;	
double				d,x,y;
TQ3Point2D			point,p1,p2;
TQ3PolarPoint		ppoint,pp1,pp2;


TQ3GroupPosition	position;
TQ3Point3D	point3;
TQ3GeometryObject	object;

if (!validWin) return MCO_FAILURE;

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

// build an arrow
TQ3GeometryObject TweakTab::NewArrow(double *lab1,double *lab2)
{
TQ3GeometryObject	geometryObject = NULL;
TQ3PolyLineData 		linedata;
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
	linedata.vertices = (TQ3Vertex3D *) NewPtr (3 * sizeof(TQ3Vertex3D));
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
	linedata.vertices = (TQ3Vertex3D *) NewPtr (3 * sizeof(TQ3Vertex3D));
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
DisposePtr ((char *) linedata.vertices);
vertices = NULL;

return	group;

bail:

if (vertices != NULL) DisposePtr ((char *) vertices);
return NULL;
}



	

TQ3GeometryObject TweakTab::NewPolyy(double *disp,double *polypoints,double scale,int num_pp,TQ3ColorRGB *color)
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


// modify a polygon

McoStatus TweakTab::ModifyPoly(double *disp,double *polypoints,double scale,int num_pp)
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


if (!validWin) return MCO_FAILURE;

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


// make a cross 

TQ3GeometryObject TweakTab::NewCross(double *lab)
{
double disp[3];
double scale;
TQ3ColorRGB 		color  = { 1.0, 1.0, 1.0 };
double CrossPoly[] = {
	-0.5,-1.5,
	0.5,-1.5,
	0.5,-0.5,
	1.5,-0.5,
	1.5,0.5,
	0.5,0.5,
	0.5,1.5,
	-0.5,1.5,
	-0.5,0.5,
	-1.5,0.5,
	-1.5,-0.5,
	-0.5,-0.5
	};
	
disp[0] = lab[1]/128;
disp[1] = lab[2]/128;
disp[2] = 0.51;

scale = 3*Document.fGroupScale;

if (scale < 1) scale = 1;
	
return NewPolyy(disp,CrossPoly,0.02/scale,12,&color);
}


// modify a cross

McoStatus TweakTab::ModifyCross(double *lab)
{
double disp[3];
double scale;
double CrossPoly[] = {
	-0.5,-1.5,
	0.5,-1.5,
	0.5,-0.5,
	1.5,-0.5,
	1.5,0.5,
	0.5,0.5,
	0.5,1.5,
	-0.5,1.5,
	-0.5,0.5,
	-1.5,0.5,
	-1.5,-0.5,
	-0.5,-0.5
	};
	
disp[0] = lab[1]/128;
disp[1] = lab[2]/128;
disp[2] = 0.51;

scale = 3*Document.fGroupScale;

if (scale < 1) scale = 1;
	
return ModifyPoly(disp,CrossPoly,0.02/scale,12);
}


	
//-------------------------------------------------------------------------------------------
// change the geometry of the object type with number num
McoStatus TweakTab::ChangeGeometry (McoQ3DType type, long num)
{
	TQ3Status			status;
	TQ3Object			object = NULL;
	TQ3GroupPosition	position,last_position = 0L;
	unsigned long		triGridLibNum;
	double				lab1[3],lab2[3];
	
	lab1[0] = 50;
	lab1[1] = 0;
	lab1[1] = 0;

	lab2[0] = 50;
	lab2[1] = 4;
	lab2[1] = 0;

	if (Document.fModel == nil)
		return MCO_OBJECT_NOT_INITIALIZED;

	status = Q3Group_GetFirstPositionOfType(Document.fModel, kQ3ShapeTypeGroup, &position);
	while (status == kQ3Success  &&  position != nil)
		{
		last_position = position;
		Q3Group_GetNextPositionOfType(Document.fModel, kQ3ShapeTypeGeometry, &position);
		}

	switch (type) {
		case MP_GamutSlice:
			object = NewGammutObject(gammutSurfaceH);
			break;
		case MP_TweakAx:
			object = New_ab_ax();
			break;	
		case MP_Circle:
			if (!current_tweak)
				object = NewCircle(lab1,10,10,SliderL);
			else if (current_tweak->type == Calibrate_Tweak)
				object = NewCircle(current_tweak->lab_g,current_tweak->Lr,current_tweak->Cr,SliderL);
			else if (current_tweak->type == GamutComp_Tweak)
				object = NewCircle(current_tweak->lab_d,current_tweak->Lr,current_tweak->Cr,SliderL);
			break;
		case MP_Arrow:
			if (!current_tweak)
				object = NewArrow(lab1,lab2);
			else if (current_tweak->type == Calibrate_Tweak)
				object = NewArrow(current_tweak->lab_g,current_tweak->lab_p);
			else if (current_tweak->type == GamutComp_Tweak)
				object = NewArrow(current_tweak->lab_d,current_tweak->lab_g);
			break;
		case MP_BlackBox:
			object = BlackBox();
			break;
		case MP_TweakMark1:
			if (current_tweak) object = NewCross(current_tweak->lab_d);
			else object = NewCross(lab1);
			break;
		case MP_TweakMark2:
	//		object = NewMark(lab2,1);
			break;
			}
			
	if (object == NULL)
		return MCO_QD3D_ERROR;
	if (last_position != nil) Q3Group_AddObjectAfter (Document.fModel,last_position, object);
	else Q3Group_AddObject (Document.fModel, object);

	Q3Object_Dispose(object);
	return MCO_SUCCESS;
}


#define bigger(a, b) ((a >= b) ? a :b)
#define smaller(a, b) ((a < b) ? a :b)

void TweakTab::_checkMaxVals(double *lab)
{
double m;

if (lab[0] > 100) lab[0] = 100;
if (lab[0] < 0) lab[0] = 0;

if (lab[1] < -128) 
	{
	m = -128/lab[1];
	lab[1] = -128;
	lab[2] = lab[2]*m;
	}
	
if (lab[1] > 127) 
	{
	m = 127/lab[1];
	lab[1] = 127;
	lab[2] = lab[2]*m;
	}	

if (lab[2] < -128) 
	{
	m = -128/lab[2];
	lab[2] = -128;
	lab[1] = lab[1]*m;
	}
	
if (lab[2] > 127) 
	{
	m = 127/lab[2];
	lab[2] = 127;
	lab[1] = lab[1]*m;
	}	

}


void TweakTab::ConvertToRGB(double *lab,double *rgb)
{
unsigned char temp[3];
double	tlab[3];

tlab[0] = lab[0];
tlab[1] = lab[1];
tlab[2] = lab[2];

_checkMaxVals(tlab);

temp[0] = 2.55*tlab[0];
temp[1] = tlab[1]+128;
temp[2] = tlab[2]+128;

	
if (lab_rgb_xform != NULL) lab_rgb_xform->evalColors(temp,temp,1);
	
rgb[0] = temp[0];
rgb[1] = temp[1];
rgb[2] = temp[2];

}


void TweakTab::ConvertToGammutC(double *lab,double *labc)
{
double	*labTable;

labTable = (double*)McoLockHandle(doc->_labtableH);

interpolate_33(lab,labc,labTable);

McoUnlockHandle(doc->_labtableH);
}


int TweakTab::isColorInGamut(double *lab)
{
unsigned char		*out_gamut;
double				lch[3];
int					j,k,l;
int					gamut;

out_gamut = (unsigned char*)McoLockHandle(doc->_out_of_gamutH);

if (lab[1] < -128) lab[1] = -128;
if (lab[1] > 127) lab[1] = 127;

if (lab[2] < -128) lab[2] = -128;
if (lab[2] > 127) lab[2] = 127;

j = 33*lab[0]/100;
k = 33*(lab[1]+128)/256;
l = 33*(lab[2]+128)/256;

if (out_gamut[l+k*33+j*33*33]) gamut = 0;
else gamut = 1;

McoUnlockHandle(doc->_out_of_gamutH);
return gamut;
}

void TweakTab::_converttoTwPrint(double *lab,double *labo)
{

doc->tweak->eval(lab,labo,1);

}

void TweakTab::_converttoTwGamut(double *lab,double *labo)
{
ConvertToGammutC(lab,labo);
doc->gtweak->eval(lab,labo,labo,1);
}


McoStatus TweakTab::SetUpDial(void)
{
	int16			large_slid_picts[] = LARGE_PICT_IDS;
	int16			large_slid_off[] = LARGE_OFFSETS;

	if (!doneGamut) return MCO_SUCCESS;

// set up the slider
	setSlider(large_slid_picts,large_slid_off,startNum+Light_slider,refCon1);	
	
	setSlider(large_slid_picts,large_slid_off,startNum+Color_slider,refCon2);
	

	return MCO_SUCCESS;
}

void TweakTab::Clear3DWin(void)
{
RGBColor	c,old_fore;

GetForeColor(&old_fore);
c.red = 0;
c.green = 0;
c.blue = 0;
RGBForeColor(&c);
PaintRect(&box_rect);
RGBForeColor(&old_fore);
}

McoStatus TweakTab::UpdateWindow(void)	
{			
PicHandle	pict;
Rect r1;
short	iType;
Handle	iHandle;
McoStatus	state;

if (doneGamut)
	{
	if (Current_Text == 1) 
		{
		unbox_item(dp,Desired_Box+startNum);
		if (current_tweak->type) 
			{
			box_item(dp,Gamut_Box+startNum);
			unbox_item(dp,Printed_Box+startNum);
			}
		else 
			{
			unbox_item(dp,Gamut_Box+startNum);
			box_item(dp,Printed_Box+startNum);
			}
		}
	else if (Current_Text == 0)
		{
		box_item(dp,Desired_Box+startNum);
		unbox_item(dp,Gamut_Box+startNum);
		unbox_item(dp,Printed_Box+startNum);
		}
	else
		{
		unbox_item(dp,Desired_Box+startNum);
		unbox_item(dp,Gamut_Box+startNum);
		unbox_item(dp,Printed_Box+startNum);
		}
	state = DrawColorBox(Desired_Box+startNum,desiredRGB);
	if (state != MCO_SUCCESS) return state;	
		
	if (current_tweak->type == Calibrate_Tweak)
		{
		state = DrawColorBox(Printed_Box+startNum,printedRGB);
		if (state != MCO_SUCCESS) return state;	
		}	

	state = DrawColorBox(Gamut_Box+startNum,gammutRGB);
	if (state != MCO_SUCCESS) return state;	

	state = DrawColorBox(Desired_View_Box+startNum,rgb_d);
	if (current_tweak)
		{
		if (current_tweak->type == Calibrate_Tweak)
			state = DrawColorBox(Tweak_View_Box+startNum,rgb_p);
		else if (current_tweak->type == GamutComp_Tweak)
			state = DrawColorBox(Tweak_View_Box+startNum,rgb_g);	
		}		
		
	if (current_tweak) QD3DWin::DrawWindow();
	else Clear3DWin();

	}
	
return MCO_SUCCESS;
}

McoStatus TweakTab::_copyTextFromDial(void)
{

	Boolean		changed = 0,changed2;
	Str255		inname;
	Rect r1;
	short	iType;
	Handle	iHandle;	
	
	if (!current_tweak) return MCO_SUCCESS;
	if (!doneGamut) return MCO_SUCCESS;

	changed = changed | GetItemTextFromNum(Desired_Start+startNum,&current_tweak->lab_d[0],0,100,0);
	changed = changed | GetItemTextFromNum(Desired_Start+startNum+1,&current_tweak->lab_d[1],-128,128,0);
	changed = changed | GetItemTextFromNum(Desired_Start+startNum+2,&current_tweak->lab_d[2],-128,128,0);
	changed2 = changed;

	changed = changed | GetItemTextFromNum(Gamut_Start+startNum,&current_tweak->lab_g[0],0,100,0);
	changed = changed | GetItemTextFromNum(Gamut_Start+startNum+1,&current_tweak->lab_g[1],-128,128,0);
	changed = changed | GetItemTextFromNum(Gamut_Start+startNum+2,&current_tweak->lab_g[2],-128,128,0);

	if (current_tweak->type == Calibrate_Tweak)
		{
		changed = changed | GetItemTextFromNum(Print_Start+startNum,&current_tweak->lab_p[0],0,100,0);
		changed = changed | GetItemTextFromNum(Print_Start+startNum+1,&current_tweak->lab_p[1],-128,128,0);
		changed = changed | GetItemTextFromNum(Print_Start+startNum+2,&current_tweak->lab_p[2],-128,128,0);
		}

	if (changed2)
		{
		if (isColorInGamut(current_tweak->lab_d)) current_tweak->type = Calibrate_Tweak;
		else current_tweak->type = GamutComp_Tweak;
		SliderL = current_tweak->lab_d[0];
		ModifyGammutObject(gammutSurfaceH);
		}
	else if ((changed) && (current_tweak->type == Calibrate_Tweak))
		{
		SliderL = current_tweak->lab_p[0];
		ModifyGammutObject(gammutSurfaceH);
		}
	else if (changed)
		{
		SliderL = current_tweak->lab_p[0];
		ModifyGammutObject(gammutSurfaceH);
		}

	if (changed) 
		{
		doc->changedProfile = 1;
		_changeTweakData(1-changed2);
		QD3DWin::DrawWindow();	

		}

		

	GetDItem (dp, Tweak_Name+startNum, &iType, (Handle*)&iHandle, &r1);
	GetIText(iHandle,inname);
	if (inname[0] > 40) inname[0] = 40;
	ptocstr(inname);
	if (strcmp(current_tweak->name,(char*)inname)) 
		{
		strcpy(current_tweak->name,(char*)inname);
		_setUpTweakMenu();
		}
	

return MCO_SUCCESS;
}


McoStatus TweakTab::_copyTextToDial(void)
{

	if (!doneGamut) return MCO_SUCCESS;

	if (!current_tweak)
		{
		SetItemText(Desired_Start+startNum,"\p   ");
		SetItemText(Desired_Start+startNum+1,"\p   ");
		SetItemText(Desired_Start+startNum+2,"\p   ");

		SetItemText(Gamut_Start+startNum,"\p   ");
		SetItemText(Gamut_Start+startNum+1,"\p   ");
		SetItemText(Gamut_Start+startNum+2,"\p   ");

		SetItemText(Gamut_Start_Ne+startNum,"\p   ");
		SetItemText(Gamut_Start_Ne+startNum+1,"\p   ");
		SetItemText(Gamut_Start_Ne+startNum+2,"\p   ");
	
		SetItemText(Print_Start+startNum,"\p   ");
		SetItemText(Print_Start+startNum+1,"\p   ");
		SetItemText(Print_Start+startNum+2,"\p   ");

		}
	else
		{

		SetItemTextToNum(Desired_Start+startNum,current_tweak->lab_d[0],0);
		SetItemTextToNum(Desired_Start+startNum+1,current_tweak->lab_d[1],0);
		SetItemTextToNum(Desired_Start+startNum+2,current_tweak->lab_d[2],0);

		SetItemTextToNum(Gamut_Start+startNum,current_tweak->lab_g[0],0);
		SetItemTextToNum(Gamut_Start+startNum+1,current_tweak->lab_g[1],0);
		SetItemTextToNum(Gamut_Start+startNum+2,current_tweak->lab_g[2],0);
		
		SetItemTextToNum(Gamut_Start_Ne+startNum,current_tweak->lab_g[0],0);
		SetItemTextToNum(Gamut_Start_Ne+startNum+1,current_tweak->lab_g[1],0);
		SetItemTextToNum(Gamut_Start_Ne+startNum+2,current_tweak->lab_g[2],0);

		if (current_tweak->type == Calibrate_Tweak)
			{
			SetItemTextToNum(Print_Start+startNum,current_tweak->lab_p[0],0);
			SetItemTextToNum(Print_Start+startNum+1,current_tweak->lab_p[1],0);
			SetItemTextToNum(Print_Start+startNum+2,current_tweak->lab_p[2],0);
			}
		}

return MCO_SUCCESS;
}

McoStatus TweakTab::updateWindowData(int changed)
{
Rect r1;
short	iType;
Handle	iHandle;
ControlHandle ControlH;
Str255	text;
char	desc[100];


if ((doc->didGammutCompression) && (!doneGamut))
	{
	doneGamut = 1;
	showWindow();
	}
if (doneGamut)
	{
	if (!current_tweak)
		{
		if (doc->printerData->num_tweaks > 0)
			{ 
			current_tweak = doc->printerData->tweaks[0];
			tweak_num = 0;
			GetDItem ( dp, Tweak_Menu+startNum, &iType, (Handle*)&ControlH, &r );
			SetCtlValue ( ControlH,tweak_num);
			_setUpTweak(1);
			_setUpTweakMenu();
			}
		}
	else
		{
		if (doc->printerData->tweaks[tweak_num] != current_tweak)
			{
			if (doc->printerData->num_tweaks > 0)
				{ 
				current_tweak = doc->printerData->tweaks[0];
				tweak_num = 0;
				GetDItem ( dp, Tweak_Menu+startNum, &iType, (Handle*)&ControlH, &r );
				SetCtlValue ( ControlH,tweak_num);
				_setUpTweak(1);
				_setUpTweakMenu();
				}
			else 
				{
				current_tweak = 0L;
				tweak_num = -1;
				_setUpTweak(1);
				_setUpTweakMenu();
				}
			}
		}
	if (!current_tweak)
		{
		Disable(dp,Light_slider+startNum);
		Disable(dp,Color_slider+startNum);
		Disable(dp,Delete_Tweak+startNum);
		Disable(dp,Gamut_Tweak_Radio+startNum);
		Disable(dp,Printing_Tweak_Radio+startNum);
		GetDItem (dp, Tweak_Name+startNum, &iType, (Handle*)&iHandle, &r1);
		SetIText(iHandle,"\p");
		}
	else
		{
		Enable(dp,Light_slider+startNum);
		Enable(dp,Color_slider+startNum);
		Enable(dp,Delete_Tweak+startNum);
		Enable(dp,Gamut_Tweak_Radio+startNum);
		Enable(dp,Printing_Tweak_Radio+startNum);
		GetDItem (dp, Tweak_Name+startNum, &iType, (Handle*)&iHandle, &r1);
		GetIText(iHandle,text);
		ptocstr(text);
		strcpy(desc,current_tweak->name);
		if (strcmp((char*)text,desc))
			{
			ctopstr(desc);
			SetIText(iHandle,(unsigned char*)desc);
			}
		}
		
	_copyTextToDial();
	}	
return MCO_SUCCESS;
}


McoStatus 	TweakTab::NewTweak(void)
{
Str255	theString,numstr;
Rect r;
Handle h;
short itemType;
ControlHandle	sliderControl;

	if (doc->printerData->num_tweaks >= MAX_NUM_TWEAKS) return MCO_FAILURE; 

	GetIndString(theString,Assorted_Names,1);

	NumToString(doc->printerData->num_tweaks+1, numstr);
	add_string(theString,numstr);
	ptocstr(theString);
	doc->printerData->tweaks[doc->printerData->num_tweaks] = new Tweak_Element((char*)theString);
	current_tweak = doc->printerData->tweaks[doc->printerData->num_tweaks];
	doc->printerData->num_tweaks++;
	
	_setUpTweakMenu();
	
	GetDItem ( dp, Tweak_Menu+startNum, &itemType, (Handle*)&sliderControl, &r );
	SetCtlValue ( sliderControl,doc->printerData->num_tweaks);
	tweak_num = doc->printerData->num_tweaks-1;
	
	_copyTextToDial();
	_updatSliderL();
	_changeTweakData(1);
	updateWindowData(0);
	
	return MCO_SUCCESS;
}

// delete the currently active tweak
McoStatus TweakTab::DeleteTweak(void)
{
int i,j;
Rect r;
short itemType;
ControlHandle	sliderControl;

	for (i=0; i<doc->printerData->num_tweaks; i++) if (doc->printerData->tweaks[i] == current_tweak)
		{
		delete current_tweak;
		for (j=i+1; j<doc->printerData->num_tweaks; j++)
			{
			doc->printerData->tweaks[j-1] = doc->printerData->tweaks[j];
			}
		doc->printerData->num_tweaks--;
	 	if (doc->printerData->num_tweaks > 0) 
	 		{
	 		current_tweak = doc->printerData->tweaks[0];
	 		tweak_num = 0;
	 		GetDItem ( dp, Tweak_Menu+startNum, &itemType, (Handle*)&sliderControl, &r );
			SetCtlValue ( sliderControl,tweak_num);
	 		}
	 	else  
	 		{
	 		current_tweak = 0L;
	 		tweak_num = -1;
	 		}
		break;
		}
	_changeTweakData(1);
	_setUpTweakMenu();
	
	updateWindowData(0);
	
	return MCO_SUCCESS;
}

McoStatus TweakTab::SaveTweakedPatches(void)
{
McoStatus state = MCO_SUCCESS;
RawData	  *tempPatchesD;

tempPatchesD = new RawData;
if (tempPatchesD == NULL) return MCO_MEM_ALLOC_ERROR;
state = tempPatchesD->copyRawData(doc->patchD);
if (state != MCO_SUCCESS) return state;


state = doc->tweak->eval(doc->patchD,tempPatchesD,doc->patchD->format.total);
if (state != MCO_SUCCESS) goto bail;
state = doc->handle_save_datafile(tempPatchesD);

bail:
delete tempPatchesD;
return state; 

}


Boolean TweakTab::isMyObject(Point where,short *item) 
{
WindowPtr oldP;
short  titem;
ControlHandle	sliderControl;

GetPort(&oldP);
SetPort(dp);
GlobalToLocal(&where);
titem = FindDItem(dp,where);
titem ++;
if (titem == LightViewSlider+startNum)
	{
	if (FindControl(where,dp,(ControlHandle*)&sliderControl) != inThumb)
		{
		*item = titem;
		SetPort(oldP);
		return TRUE;
		}
	}
SetPort(oldP);
return FALSE;
}

Boolean TweakTab::TimerEventPresent(void)
{
if( TickCount() - _last_tick_time2 > 4 ) 
	{
	return TRUE;
	}
return FALSE;
}


McoStatus TweakTab::get3Dinfo(Point clickPoint,double *lab,int gamutCheck,int *ingamut)
{

	TQ3WindowPointPickData			myWPPickData;
	TQ3PickObject						myPickObject;
	TQ3PickDetail						validMask;
	unsigned long						myNumHits;
	unsigned long						myIndex;
	Point								myPoint;
	TQ3Point2D							my2DPoint;
	TQ3ViewObject						myView;
	long 								state;
	TQ3Point3D							point,LPoint;
	int									get_point;
	TQ3Object							object = 0L;
	TQ3ShapePartObject					shapePart;
	TQ3Matrix4x4 						LtoG;

	// Get the window coordinates of a mouse click.
	SetPort(dp);
	myPoint = clickPoint;				// get location of mouse click
	GlobalToLocal(&myPoint);				// convert to window coordinates
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
	
	
		Q3Pick_GetPickDetailData(myPickObject, myIndex, kQ3PickDetailMaskObject, &object);
		// operate on myHitData, then...
		//SysBeep(10);	
		
		get_point = 1;
		if (Q3Geometry_GetType(object) == kQ3GeometryTypeMesh)
			*ingamut = 1;
		else *ingamut = 0;
		
		if ((object) && (gamutCheck))
			{
			if (*ingamut)
				{
				get_point = 1;
				}
			else 
				{
				get_point = 0;
				}
			}
			
		if (get_point)
			{	
			Q3Pick_GetPickDetailData(myPickObject, myIndex, kQ3PickDetailMaskLocalToWorldMatrix, &LtoG);
			Q3Pick_GetPickDetailData(myPickObject, myIndex, kQ3PickDetailMaskXYZ, &point);
			translateGlobalToLocal(&LtoG,&point,&LPoint);
			lab[0] = SliderL;
			lab[1] = 128*LPoint.x;
			lab[2] = 128*LPoint.y;
			}
		if (object) Q3Object_Dispose(object);

	}

	// Dispose of all hits in the hit list.
	Q3Pick_EmptyHitList(myPickObject);

	// Dispose of the pick object.
	Q3Object_Dispose(myPickObject);
bail:
	return MCO_SUCCESS;
}

// Get the active region
McoStatus TweakTab::makeEventRegion(RgnHandle previewRgn,RgnHandle mouseRgn)
{
Rect r;
Handle h;
short itemType;

GetDItem (dp,View_Box+startNum, &itemType, &h, &r);

RectRgn(previewRgn,&r);
XorRgn(mouseRgn,previewRgn,mouseRgn);

return MCO_SUCCESS;
}

McoStatus TweakTab::doPointInWindowActiveRgn(Point where,WinEve_Cursors &cursortype)
{
McoStatus state = MCO_SUCCESS;
unsigned char 	*previewOutBuf;
unsigned char 	*previewInBuf;
GrafPtr			oldGraf;
Rect r1;
Handle h;
short itemType;
double  labc[3];
double	lab[3];
double	labp[3];
double  rgb[3];
Point	tp;
int		ingamut = 0;

WindowPtr	oldP;
	
GetPort(&oldP);
SetPort( dp ) ;

tp = where;

GlobalToLocal(&tp);
if ( (PtInRect(tp,&box_rect)) && (current_tweak))
	{
	if (current_tweak->type == GamutComp_Tweak)
		{
		get3Dinfo(where,lab,0,&ingamut);
		ingamut = 1-ingamut;
		}
	else if (current_tweak->type == Calibrate_Tweak)
		get3Dinfo(where,lab,1,&ingamut);

	if (ingamut)
		{
		ConvertToRGB(lab,rgb_d);
		_converttoTwGamut(lab,labc);
		ConvertToRGB(labc,rgb_g);
		ConvertToGammutC(lab,labp);
		_converttoTwPrint(labp,labp);
		ConvertToRGB(labp,rgb_p);

		state = DrawColorBox(Desired_View_Box+startNum,rgb_d);

		SetItemTextToNum(Desired_View_Text_St+startNum,lab[0],0);
		SetItemTextToNum(Desired_View_Text_St+startNum+1,lab[1],0);
		SetItemTextToNum(Desired_View_Text_St+startNum+2,lab[2],0);
		
		if (current_tweak->type == GamutComp_Tweak)
			{
			state = DrawColorBox(Tweak_View_Box+startNum,rgb_g);
			SetItemTextToNum(Tweak_View_Text_St+startNum,labc[0],0);
			SetItemTextToNum(Tweak_View_Text_St+startNum+1,labc[1],0);
			SetItemTextToNum(Tweak_View_Text_St+startNum+2,labc[2],0);
			}
		else if (current_tweak->type == Calibrate_Tweak)
			{
			state = DrawColorBox(Tweak_View_Box+startNum,rgb_p);
			SetItemTextToNum(Tweak_View_Text_St+startNum,labp[0],0);
			SetItemTextToNum(Tweak_View_Text_St+startNum+1,labp[1],0);
			SetItemTextToNum(Tweak_View_Text_St+startNum+2,labp[2],0);
			}
		}
	}
SetPort(oldP);
return state;
} 


// The event handler for the main menu
McoStatus	TweakTab::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed)
{
StandardFileReply		theReply;
SFTypeList typeList;
FileFormat *file;
McoStatus state = MCO_SUCCESS;
Rect r;
Handle h;
short itemType;
ControlHandle	sliderControl;
long num;
int	Changed = 0;
double temp;
Point		mpoint;
Window_Events 	code2;
int		ingamut;

short item2 = item - startNum;

	// no codes passed back
	*numwc = 0;
	
	
	if (item2 == Tweak_Menu)
		{
		GetDItem ( dp, item, &itemType, (Handle*)&sliderControl, &r );
		num = GetCtlValue ( sliderControl)-1;
		if (doc->printerData->tweaks[num]) current_tweak = doc->printerData->tweaks[num];
		tweak_num = num;
		_changeTweakData(1);
		updateWindowData(0);
		}
	else if (item2 == Desired_Box)
		{
		Enable(dp,Current_Button);
		Current_Text = 0;
		SliderL = current_tweak->lab_d[0];
		_updatSliderL();
		UpdateWindow();
		}
	else if (item2 == Gamut_Box)
		{
		if (current_tweak)
			if (current_tweak->type) 
				{
				Enable(dp,Current_Button);
				Current_Text = 1;
				SliderL = current_tweak->lab_g[0];
				_updatSliderL();
				UpdateWindow();
				}
		}
	else if (item2 == Printed_Box)
		{
		if (current_tweak)
			if (!current_tweak->type) 
				{
				Enable(dp,Current_Button);
				Current_Text = 1;
				SliderL = current_tweak->lab_p[0];
				_updatSliderL();
				UpdateWindow();
				}
		}
	else if (item2 == Gamut_Tweak_Radio)
		{
		if (current_tweak)
			{
			GetDItem ( dp, item, &itemType, (Handle*)&sliderControl, &r );
			SetCtlValue (sliderControl, 1);
			GetDItem ( dp, Printing_Tweak_Radio+startNum, &itemType, (Handle*)&sliderControl, &r );
			SetCtlValue (sliderControl, 0);
			current_tweak->type = GamutComp_Tweak;
			_changeTweakData(1);
			Changed = TRUE;
			}
		}
	else if (item2 == Printing_Tweak_Radio)
		{
		if (current_tweak)
			{
			GetDItem ( dp, item, &itemType, (Handle*)&sliderControl, &r );
			SetCtlValue (sliderControl, 1);
			GetDItem ( dp, Gamut_Tweak_Radio+startNum, &itemType, (Handle*)&sliderControl, &r );
			SetCtlValue (sliderControl, 0);
			current_tweak->type = Calibrate_Tweak;
			_changeTweakData(1);
			Changed = TRUE;
			}
		}		
	else if (item2 == Light_slider)
		{
		GetDItem ( dp, item, &itemType, (Handle*)&sliderControl, &r );
		temp = current_tweak->Lr;
		if (current_tweak) current_tweak->Lr = GetCtlValue ( sliderControl);
		if (temp != current_tweak->Lr) changed = 1;
		if (current_tweak->type == Calibrate_Tweak)
			ModifyCircle(current_tweak->lab_g,current_tweak->Lr,current_tweak->Cr,SliderL);
		else if (current_tweak->type == GamutComp_Tweak)
			ModifyCircle(current_tweak->lab_d,current_tweak->Lr,current_tweak->Cr,SliderL);
		QD3DWin::DrawWindow();
		Changed = TRUE;	
		}
	else if (item2 == Color_slider)
		{
		GetDItem ( dp, item, &itemType, (Handle*)&sliderControl, &r );
		temp = current_tweak->Cr;
		if (current_tweak) current_tweak->Cr = GetCtlValue ( sliderControl);
		if (temp != current_tweak->Cr) changed = 1;
		if (current_tweak->type == Calibrate_Tweak)
			ModifyCircle(current_tweak->lab_g,current_tweak->Lr,current_tweak->Cr,SliderL);
		else if (current_tweak->type == GamutComp_Tweak)
			ModifyCircle(current_tweak->lab_d,current_tweak->Lr,current_tweak->Cr,SliderL);
		QD3DWin::DrawWindow();
		Changed = TRUE;	
		}	
	else if (item2 == New_Tweak)
		{
		NewTweak();
		Changed = 1;
		}
	else if (item2 == Delete_Tweak)
		{
		DeleteTweak();
		Changed = 1;
		}
	else if (item2 == LightViewSlider)
		{
		GetMouse(&mpoint);
		do {
			SliderL = GetSlider(mpoint,item);
			_updatSliderL();
			QD3DWin::DrawWindow();
			GetMouse(&mpoint);
			} while ( StillDown ( ) );
		}
	if (item2 == Zoom_Button)
		{
		Enable(dp,Current_Button);
		Disable(dp,Zoom_Button+startNum);
		Current_Button = item;
		Current_Text = -1;
		UpdateWindow();
		}
	else if (item2 == Move_Button)
		{
		Enable(dp,Current_Button);
		Disable(dp,Move_Button+startNum);
		Current_Button = item;
		Current_Text = -1;
		UpdateWindow();
		}
	else if ((item2 == View_Box) && (current_tweak))
		{
		if (Current_Text == -1)
			{
			if (Current_Button == Move_Button+startNum)
				{
				code2 = WE_Translate;
				}
			else
				{
				code2 = WE_Scale;
				}
			ProcessMouseDown(clickPoint,code2);
			ModifyCross(current_tweak->lab_d);
			QD3DWin::DrawWindow();
			}
		else
			{
			if (Current_Text == 0)
				{
				state = get3Dinfo(clickPoint,current_tweak->lab_d,0,&ingamut);
				if (ingamut) current_tweak->type = Calibrate_Tweak;
				else current_tweak->type = GamutComp_Tweak;
				}
			else if (Current_Text == 1)
				{
				if (current_tweak->type == Calibrate_Tweak)
					state = get3Dinfo(clickPoint,current_tweak->lab_p,1,&ingamut);
				else 
					state = get3Dinfo(clickPoint,current_tweak->lab_g,1,&ingamut);
				}
			Changed = TRUE;
			_changeTweakData(Current_Text);
			UpdateWindow();
			}
		}
	else if (item == TIMER_ITEM ) 
		{   // a timer event has occured
		_last_tick_time2 = TickCount();
		if (doc->tcom) if (!doc->tcom->IsBufferEmpty())	
			{
			if (doc->tcom->IsDataPresent())
				{
				if (Current_Text == 0)
					{
					state = doc->tcom->getNextPatch(current_tweak->lab_d);
					SliderL = current_tweak->lab_d[0];
					if (isColorInGamut(current_tweak->lab_d)) current_tweak->type = Calibrate_Tweak;
					else current_tweak->type = GamutComp_Tweak;
					_updatSliderL();
					_changeTweakData(0);

					}
				else if (Current_Text == 1)
					{
					if (current_tweak->type == Calibrate_Tweak)
						{
						state = doc->tcom->getNextPatch(current_tweak->lab_p);
						SliderL = current_tweak->lab_p[0];
						}
					else
						{
						state = doc->tcom->getNextPatch(current_tweak->lab_g);
						SliderL = current_tweak->lab_g[0];
						}
					_updatSliderL();
					_changeTweakData(1);

					}
				Changed = TRUE;
				if (state != MCO_SUCCESS) McoErrorAlert(state);
				state = MCO_SUCCESS;
				}
			}
		}
	else state = _copyTextFromDial();
	
	if (Changed) 
		{
		doc->changedProfile = 1;
		_setUpTweaks();
		}
	changed = Changed;	
	return state;
}