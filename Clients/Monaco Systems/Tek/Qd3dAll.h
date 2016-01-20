#pragma once

#include "QD3D.h"
#include "QD3DErrors.h"
#include "QD3DDrawContext.h"
#include "QD3DRenderer.h"
#include "QD3DShader.h"
#include "QD3DCamera.h"
#include "QD3DLight.h"
#include "QD3DGeometry.h"
#include "QD3DGroup.h"
#include "QD3DMath.h"
#include "QD3DTransform.h"
#include "QD3DStorage.h"
#include "QD3DIO.h"
#include "QD3DAcceleration.h"
#include "QD3DSet.h"
#include "Qd3dWinViewer.h"
#include "Qd3dPick.h"

#include "mcotypes.h"
#include "mcostat.h"
#include "mcomem.h"

typedef enum {
	WE_Rotate,
	WE_Scale,
	WE_Translate,
	} Window_Events;
	
typedef struct _documentRecord {
	TQ3ViewObject	fView;					// the view for the scene
	TQ3GroupObject	fModel;					// object in the scene being modelled
	TQ3StyleObject	fInterpolation;			// interpolation style used when rendering
	TQ3StyleObject	fBackFacing;			// whether to draw shapes that face away from the camera
	TQ3StyleObject	fFillStyle;				// whether drawn as solid filled object or decomposed to components
	TQ3ShaderObject	fIllumination;
	TQ3Point3D		fGroupCenter;			// the center point of the main group
	double			fGroupScale;			// how much to scale for the camera
	TQ3Matrix4x4	fRotation;				// the rotation transform for the model
	TQ3Point3D		fCameraFrom;			// the camera location
	TQ3Point3D		fCameraTo;				// the point the camera is pointing
} DocumentRec;

typedef enum
{
	MP_GammutSurface = 0, 	// A gammut surface object
	MP_CIELabRef_PG,		// A reference object in polygon form
	MP_CIELabRef_PL,		// A reference object in line form
	MP_CIEFreq,				// Labels for the CIE refrence
	MP_LxyAx,				// Lxy axies
	MP_LxyBox,				// Lxy Box
	MP_Logo,				// MonacoPRINT logo
	MP_GamutSlice,			// A slice through the gamut as used by tweak
	MP_TweakAx,				// a & b axies for tweak
	MP_Circle,				// the circle used in tweak
	MP_Arrow,				// the arrow used in tweak
	MP_BlackBox,
	MP_TweakMark1,			// the first tweak mark
	MP_TweakMark2			// the second tweak mark
} Qd3dType;
	
