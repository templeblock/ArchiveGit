/*********************************************/
/* qd3Dwin.c							 	 */
/* Display 3D  in window					 */
/* Jan 19, 1996							     */
/*********************************************/
/*   James Vogh 							 */
/*********************************************/

#include <QD3DGeometry.h>
#include "qd3Dwin.h"
#include "mcostat.h"

#include <Memory.h>
#include "math.h"
#include "colortran.h"
#include "mcofile.h"

#include "AttributeSet_Lib.h"

#include "MathUtilities.h"




McoStatus QD3DWin::InitDocumentData( DocumentPtr theDocument ) 
{
	GWorldPtr		theOffscreen ;
	GDHandle		theDevice ;
	TQ3Point3D		myOrigin = { 0, 0, 0 } ;
	
	// create a GWorld the size of the window area
/*	OSErr myErr = NewGWorld(	&theDocument->fGWorld,
								32,
								&box_rect,
								nil,
								nil,
								0L );
	
	if(myErr != noErr )
		goto bail ; 
		
	GetGWorld( &theOffscreen, &theDevice ) ;
	SetGWorld( theDocument->fGWorld, nil ) ;
	EraseRect( &box_rect ) ;
	SetGWorld( theOffscreen, theDevice ) ; */
	
	// sets up the 3d data for the scene
	//	Create view for QuickDraw 3D.
	theDocument->fView = MyNewView( dp,&box_rect,ambient,point,fill,&theDocument->fCameraFrom,&theDocument->fCameraTo);

	// the main display group:
	theDocument->fModel = MyNewModel();
	
	// scale and group center
	theDocument->fGroupScale = 1;				
	theDocument->fGroupCenter = myOrigin ;	
	
	// the drawing styles:
	theDocument->fInterpolation = Q3InterpolationStyle_New(kQ3InterpolationStyleVertex) ;
	theDocument->fBackFacing = Q3BackfacingStyle_New( kQ3BackfacingStyleBoth ) ;
	theDocument->fFillStyle = Q3FillStyle_New(kQ3FillStyleFilled ) ;

	// set the rotation matrix the identity matrix
	Q3Matrix4x4_SetIdentity(&theDocument->fRotation);	
					
	return MCO_SUCCESS;
	
bail:
	// we failed setting up the GWorld
	// so we want to quit here
	//ExitToShell() ;
	return MCO_FAILURE;
}


void QD3DWin::DisposeDocumentData( DocumentPtr theDocument)
{
	Q3Object_Dispose(theDocument->fView) ;				// the view for the scene
	Q3Object_Dispose(theDocument->fModel) ;				// object in the scene being modelled
	Q3Object_Dispose(theDocument->fInterpolation) ;		// interpolation style used when rendering
	Q3Object_Dispose(theDocument->fBackFacing) ;		// whether to draw shapes that face away from the camera
	Q3Object_Dispose(theDocument->fFillStyle) ;			// whether drawn as solid filled object or decomposed to components
}
//-----------------------------------------------------------------------------
// assumes the port is set up before being called

TQ3Status QD3DWin::DocumentDraw3DData( DocumentPtr theDocument )
{	
	TQ3Status theStatus ;	

	//	Start rendering.
	Q3View_StartRendering(theDocument->fView) ;
	do {
		theStatus = SubmitScene( theDocument ) ;
	} while (Q3View_EndRendering(theDocument->fView) == kQ3ViewStatusRetraverse );

	return theStatus ;	
}

//-------------------------------------------------------------------------------------------
//

void QD3DWin::DocumentDrawOnscreen(DocumentPtr theDocument, Rect *clipRect)
{
RGBColor	c,oldbackcolor;

/*	GetBackColor(&oldbackcolor);
	c.red = 65535;
	c.green = 65535;
	c.blue = 65535;
	RGBBackColor(&c);

	if (theDocument->fGWorld) {
	

	
		CGrafPtr			savedPort;
		GDHandle			savedDevice;

		GetGWorld( &savedPort, &savedDevice);
		SetGWorld( (CGrafPtr)the_window,  nil);
		
		ClipRect( clipRect ) ;
		
		// don't need to lockPixels on the GWorld as the 
		// offscreen remains locked (see IM: QD3D), the
		// pixmap for a pixmap draw context must remain locked
		
		CopyBits ((BitMapPtr) &theDocument->fGWorld->portPixMap,
				  &the_window->portBits,
				  &theDocument->fGWorld->portRect, 
				  &box_rect,
				  srcCopy + ditherCopy, 
				  0L);
				  
		SetGWorld( savedPort, savedDevice);
  	}
  	RGBBackColor(&oldbackcolor); */
  	
}



// initialize the 3D and load an object
QD3DWin::QD3DWin() 
{

num_objects = 0;

}

QD3DWin::~QD3DWin(void) 
{
int i;
	TQ3Status	myStatus;
	DisposeDocumentData( &Document ) ;

	//	Close our connection to the QuickDraw 3D library
	//myStatus = Q3Exit();
	//CloseDialog(dp);
	//if (the_window != NULL) DisposeWindow(the_window);

	for (i=0; i<num_objects; i++) if (objects[i]) delete objects[i];

}		
		
		
McoStatus QD3DWin::DrawWindow(void)	
{			
	Rect updateRect;
	RGBColor	c,oldbackcolor;
	WindowPtr	oldp;

	// update the window data	
	GetBackColor(&oldbackcolor);
	updateRect = (**(the_window->visRgn)).rgnBBox;
	GetPort(&oldp);
	SetPort( the_window ) ;
	DocumentDraw3DData( &Document ) ;
	DocumentDrawOnscreen( &Document, &box_rect ) ;
	RGBBackColor(&oldbackcolor);
	SetPort(oldp);
	
	return MCO_SUCCESS;
}	


// update the data within the window
McoStatus QD3DWin::forceUpdateData(void)
{
SetPort( the_window ) ;
InvalRect(&box_rect);
return MCO_SUCCESS;
}
		
McoStatus QD3DWin::UpdateWindow(void)	
{			
	Rect updateRect;
	WindowPtr	oldp;
	RGBColor	c,oldbackcolor;
	
	// update the window data	
	updateRect = (**(the_window->visRgn)).rgnBBox;
	GetPort(&oldp);
	SetPort( the_window ) ;
	GetBackColor(&oldbackcolor);
	BeginUpdate( the_window );
	DocumentDraw3DData( &Document ) ;
	DocumentDrawOnscreen( &Document, &updateRect ) ;
	RGBBackColor(&oldbackcolor);
	DrawDialog( the_window );
	EndUpdate( the_window );
	SetPort(oldp);
	
	return MCO_SUCCESS;
}			
			



// Create a marker from a string

TQ3GeometryObject QD3DWin::MarkerFromText(unsigned char *st,double x, double y, double z)
{
	GDHandle				oldGD;
	GWorldPtr				oldGW;
	unsigned short 			height = 18;
	unsigned short			height2 = 40;
	unsigned short			width,width2;
	unsigned char			*memory;
	unsigned long			memoryAddr;
	unsigned long			memoryRowBytes;
	TQ3MarkerData			markerData;
	TQ3Object				myMarker = NULL;
	Rect 					rectGW;
	QDErr					err;
	GWorldPtr 				pGWorld;
	unsigned char 			*pictMap;
	unsigned long			pictMapAddr;
	unsigned long 			pictRowBytes;
	PixMapHandle 			hPixMap;
	register unsigned long	row;
	register unsigned long	col;
	unsigned char 			temp;
	
	width = StringWidth(st);
	if (width < 40 ) width2 = 40;
	else width2 = width;
	

	GetGWorld(&oldGW, &oldGD);

	/* create the GWorld */
	SetRect(&rectGW, 0, 0, height2, width2);

	err = NewGWorld(&pGWorld, 32, &rectGW, 0, 0, useTempMem);
	if (err != noErr)
		return 0;
	
	hPixMap = GetGWorldPixMap(pGWorld);
	pictMapAddr = (unsigned long)GetPixBaseAddr (hPixMap);
	pictRowBytes = (unsigned long)(**hPixMap).rowBytes & 0x7fff;
	
	/* put the PICT into the window */
	SetGWorld(pGWorld, nil);
	
	LockPixels(hPixMap);
	EraseRect(&rectGW);
	MoveTo(0,12);
	DrawString(st);
		
	memoryRowBytes = Q3Bitmap_GetImageSize(width, height)/height;	
		
	/* allocate an area of memory for the texture */
	memory = (unsigned char *)McoMalloc(height*memoryRowBytes);
	if (memory == NULL) {
		goto bail;
	}
	/* bMap->image = (char *)textureMap; */

	/* copy the PICT into the texture */
	memoryAddr = (unsigned long)memory;
	for (row = 0L; row < height; row++) {
		pictMap = (unsigned char *)(pictMapAddr + (pictRowBytes * row));
		memory = (unsigned char *)(memoryAddr + (memoryRowBytes * row));
		for (col = 0L; col < width; col+=8) {
			pictMap+=3;
			temp = (*pictMap++) & 0x80;
			pictMap+=3;
			temp += (*pictMap++) & 0x40;
			pictMap+=3;
			temp += (*pictMap++) & 0x20;
			pictMap+=3;
			temp += (*pictMap++) & 0x10;
			pictMap+=3;
			temp += (*pictMap++) & 0x08;
			pictMap+=3;
			temp += (*pictMap++) & 0x04;
			pictMap+=3;
			temp += (*pictMap++) & 0x02;
			pictMap+=3;
			temp += (*pictMap++) & 0x01;
			*memory++ = temp ^ 0xFF;
			//if (row % 2) *memory++ = 255;
			//else *memory++ = 0;
		}
	}		

memory = (unsigned char *)memoryAddr;					
// set up the marker
Q3Point3D_Set(&markerData.location, x, y, z);
markerData.xOffset			= 0;
markerData.yOffset			= 0;
markerData.bitmap.image		= memory;
markerData.bitmap.width		= width;
markerData.bitmap.height	= height;
markerData.bitmap.rowBytes	= memoryRowBytes;
markerData.bitmap.bitOrder	= kQ3EndianBig;
markerData.markerAttributeSet = NULL;

myMarker = Q3Marker_New(&markerData);

	
McoFree(memory);		

bail:		

SetGWorld(oldGW, oldGD);

DisposeGWorld(pGWorld);
return (myMarker);		
		
}

/* update the data in the window */
McoStatus  QD3DWin::updateWindowData(int changed)
{
//DocumentDraw3DData( &Document ) ;
return MCO_SUCCESS;
}			


void QD3DWin::DoTransform(int code, double v1, double v2,double v3)
{
TQ3Matrix4x4	tmp;
float		scale,xr,yr,dx,dy;
float		xt,yt,zt;
RGBColor	c,oldbackcolor;

if (code  == WE_Rotate)
	{
	Q3Matrix4x4_SetRotate_XYZ(&tmp, v1, v2, v3);
	Q3Matrix4x4_Multiply(&Document.fRotation, &tmp, &Document.fRotation);
	//DocumentDraw3DData( &Document ) ;
	//DrawWindow();
	}
else if (code == WE_Scale)
	{
	Document.fGroupScale += v1;
	if (Document.fGroupScale<0) Document.fGroupScale = 0;
	//DocumentDraw3DData( &Document ) ;
	//DrawWindow();
	}
else if (code == WE_Translate)
	{
	
	xt = -Document.fRotation.value[0][0]*v1+Document.fRotation.value[1][0]*v2;
	yt = -Document.fRotation.value[0][1]*v1+Document.fRotation.value[1][1]*v2;
	zt = Document.fRotation.value[0][2]*v1-Document.fRotation.value[1][2]*v2;
	Document.fGroupCenter.x += xt;
	Document.fGroupCenter.y += yt;
	Document.fGroupCenter.z += zt;
	//DocumentDraw3DData( &Document ) ;
	//DrawWindow();
	}
}


McoStatus QD3DWin::ProcessMouseDown(Point ClickPoint,int code)
{
int i;
Point	where,last;
double	xrt,yrt;
double 	scalet,dxt,dyt;

where = ClickPoint;
if (dp != NULL) SetPort(dp);
GlobalToLocal(&where);
last = where;
if (code == WE_Rotate)
	{
	do {
		xrt = (where.h - last.h);
		xrt = xrt/100;
		yrt = (where.v - last.v);
		yrt = yrt/100;
		DoTransform(code,yrt,xrt,0);
		updateWindowData(0);
		DrawWindow();
		last = where;
		GetMouse ( &where );
		} while ( StillDown ( ) );
	}
else if (code == WE_Scale)
	{
	do {
		scalet = (where.v - last.v);
		scalet = scalet/200;
		DoTransform(code,scalet,0,0);
		updateWindowData(0);
		DrawWindow();
		last = where;
		GetMouse ( &where );
		} while ( StillDown ( ) );
	}
else if (code == WE_Translate)
	{
		do {
		dxt = (where.h - last.h);
		dxt = dxt/175;
		dyt = (where.v - last.v);
		dyt = dyt/175;
		DoTransform(code,dxt,dyt,0);
		updateWindowData(0);
		DrawWindow();
		last = where;
		GetMouse ( &where );
		} while ( StillDown ( ) );
	}
return MCO_SUCCESS;
}


//-------------------------------------------------------------------------------------------
//
McoStatus QD3DWin::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed)
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
//		Enable(dp,Current_Button);
//		Disable(dp,ids[QD3D_Info]);
//		Current_Button = item;
		*numwc = 1;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].code = WE_Stop;
		} 
	else if (item == ids[QD3D_Box])
		{
		*numwc = 1;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].doc = 0L;
		if (Current_Button == ids[QD3D_Rotate])
			{
			wc[0].code = WE_Rotate;
			}
		else if (Current_Button == ids[QD3D_Zoom])
			{
			wc[0].code = WE_Scale;
			}
		else if (Current_Button == ids[QD3D_Translate])
			{
			wc[0].code = WE_Translate;
			}
//		else if (Current_Button == ids[QD3D_Info])
//			{
//			status = get3Dinfo(clickPoint);
//			}
		//ProcessMouseDown(clickPoint,code2);
		}
return status;	
}

void QD3DWin::GetRotation(TQ3Matrix4x4 *rotation)
{
Q3Matrix4x4_Copy(&Document.fRotation, rotation);
}

void QD3DWin::SetRotation(TQ3Matrix4x4 *rotation)
{
Q3Matrix4x4_Copy(rotation, &Document.fRotation);
}




McoQ3DObject* QD3DWin::getObject(TQ3Object object)
{
int i;

for (i=0; i<num_objects; i++)
	{
	if (objects[i]->IsObject(object)) return objects[i];
	}
return NULL;
}


void	QD3DWin::addObject(TQ3Object object,McoQ3DType type,long num)
{
objects[num_objects] = new McoQ3DObject(object,type,num);
num_objects++;
}

void	QD3DWin::removeObject(TQ3Object object)
{
int i,j;

for (i=0; i<num_objects; i++)
	{
	if (objects[i]->IsObject(object)) break;
	}
if (i == num_objects) return;

delete objects[i];
for (j=i; j<num_objects; j++)
	{
	objects[j] = objects[j+1];
	}
num_objects--;
}



McoQ3DObject::McoQ3DObject(TQ3Object o, McoQ3DType t, long n)
{
object = o;
type = t;
num = n;
}

Boolean McoQ3DObject::IsObject(TQ3Object o)
{
if (object == o) return TRUE;
return FALSE;
}

Boolean McoQ3DObject::IsTypeNum(McoQ3DType t, long n)
{
if ((type == t) && (num == n)) return TRUE;
return FALSE;
}


