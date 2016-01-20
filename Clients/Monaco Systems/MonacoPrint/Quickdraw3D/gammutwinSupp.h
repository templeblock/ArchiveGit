// Quickdraw 3D sample code
//
// Nick Thompson, AppleLink: DEVSUPPORT (devsupport@applelink.apple.com)
//
// ©1994-5 Apple Computer Inc., All Rights Reserved


#ifndef _MY3DSUPPORT_H_
#define _MY3DSUPPORT_H_

// Macintosh System Stuff
#include <Types.h>
#include <Windows.h>

#include <QDOffScreen.h>

// QuickDraw 3D stuff
#include "QD3D.h"
#include "QD3DErrors.h"
#include "QD3DView.h"

//-------------------------------------------------------------------------------------------

struct _documentRecord {
	GWorldPtr		fGWorld ;				// the offscreen used as the basis for this apps
											// pixmap draw context
	TQ3ViewObject	fView ;					// the view for the scene
	TQ3GroupObject	fModel ;				// object in the scene being modelled
	TQ3StyleObject	fInterpolation ;		// interpolation style used when rendering
	TQ3StyleObject	fBackFacing ;			// whether to draw shapes that face away from the camera
	TQ3StyleObject	fFillStyle ;			// whether drawn as solid filled object or decomposed to components
	TQ3Point3D		fGroupCenter ;			// the center point of the main group
	float			fGroupScale ;			// how much to scale for the camera
	TQ3Matrix4x4	fRotation;				// the rotation transform for the model
	TQ3Point3D		fCameraFrom;			// the camera location
	TQ3Point3D		fCameraTo;				// the point the camera is pointing
};

typedef struct _documentRecord DocumentRec, *DocumentPtr, **DocumentHdl ;

//---------------------------------------------------------------------------------------

OSErr MyQD3DInitialize( void ) ;
OSErr MyQD3DExit() ;

TQ3ViewObject		MyNewView(WindowPtr dp,Rect *winrect,float ambient, float point, float fill,TQ3Point3D *from, TQ3Point3D *to);
TQ3DrawContextObject MyNewDrawContext(WindowPtr dp,Rect *winrect) ;
TQ3CameraObject		MyNewCamera(Rect *winrect,TQ3Point3D *cameraFrom,TQ3Point3D *cameraTo);
TQ3GroupObject		MyNewLights(float ambient, float point, float fill) ;
TQ3GroupObject		MyNewModelFromFile(FSSpec *theFileSpec) ;
TQ3Status 			SubmitScene( DocumentPtr theDocument ) ;
TQ3GroupObject 		MyNewModel(void) ;

void translateGlobalToLocal(TQ3Matrix4x4 *LtoG, TQ3Point3D *GPoint,TQ3Point3D *LPoint);

TQ3Point3D AdjustCamera(
	DocumentPtr			theDocument,
	short				winWidth,
	short				winHeight) ;
	
#endif
