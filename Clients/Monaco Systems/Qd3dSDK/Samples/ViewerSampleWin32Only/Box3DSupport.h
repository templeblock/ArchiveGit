// Box3dSupport.c - QuickDraw 3d routines - interface
//
// This file contains
//
// Created 27th Dec 1994, Nick Thompson, DEVSUPPORT
//
// Modification History:
//
//	12/27/94		nick		initial version

#ifndef _BOX3DSUPPORT_H_
#define _BOX3DSUPPORT_H_

#include <windows.h>

// QuickDraw 3D stuff
#include "QD3D.h"			// QuickDraw 3D lives in here
#include "QD3DIO.h"
#include "QD3DErrors.h"
#include "QD3DMath.h"
#include "QD3DDrawContext.h"
#include "QD3DShader.h"
#include "QD3DTransform.h"
#include "QD3DGroup.h"
#include "QD3DRenderer.h"
#include "QD3DCamera.h"
#include "QD3DLight.h"
#include "QD3DGeometry.h"
#include "QD3DSet.h"
#include "QD3DAcceleration.h"
#include "QD3DStorage.h"
#include "QD3DView.h"

#include "QD3DWinViewer.h"

enum
{
	kA3BtnZoom		= 1,
	kA3BtnRotate,
	kA3BtnPan
};

struct _documentRecord
{
	TQ3Boolean		fControlStripVisible;	// if control strip is visible
	TQ3ViewObject	fView ;				// the view for the scene
	TQ3GroupObject	fModel ;			// object in the scene being modelled
	TQ3StyleObject	fInterpolation ;	// interpolation style used when rendering
	TQ3StyleObject	fBackFacing ;		// whether to draw shapes that face away from the camera
	TQ3StyleObject	fFillStyle ;		// whether drawn as solid filled object or decomposed to components
	TQ3Matrix4x4	fRotation;			// the transform for the model
	TQ3ShaderObject	fIllumination;

	HWND			fWindow;			// destination window to blit offscreen buffer onto
	unsigned long	fWidth;
	unsigned long	fHeight;
	HDC				fMemoryDC;			// memory device context for offscreen buffer
	HBITMAP			fBitmap;			// offscreen bitmap to draw into (really a pixelmap)
	void			*fBitStorage;		// storage for bits in offscreen bitmap (pixelmap)

	TQ3WinViewerObject	fViewer;

	unsigned long	fViewerMode;		/* which button is active */

	TQ3Boolean		fMouseDown;
};

typedef struct _documentRecord DocumentRec, *DocumentPtr ;

//---------------------------------------------------------------------------------------

TQ3ViewObject 			MyNewView(DocumentPtr theDocument) ;
TQ3DrawContextObject	MyNewDrawContext(DocumentPtr theDocument) ;
TQ3CameraObject 		MyNewCamera(DocumentPtr theDocument) ;
TQ3GroupObject			MyNewLights(void) ;
TQ3GroupObject 			MyNewModel(void) ;

void pvCamera_Fit(DocumentPtr theDocument);
void pvBBoxCenter(TQ3BoundingBox *bbox, TQ3Point3D *center);
void pvBBox_Get(DocumentPtr theDocument, TQ3BoundingBox *bbox);

#endif
