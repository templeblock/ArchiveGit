#include "mask.h"		// for MAX_BEZIER_IN

typedef struct _shapeedit
	{
	LPSHAPE lpEditShape;	// Shape list to edit
	LPSHAPE lpActiveShape;	// Active shape in shape list
	int PointGrabbed; 		// Point which wwas hit by mouse
	int ActivePoint;		// Active(filled) point
	int ActiveControl[MAX_BEZIER_IN][2]; // pairs of active points and thier control points
	POINT OrigControlPos[MAX_BEZIER_IN][2];	// The original postions of ActiveControl[i][j]
	int numActiveControl;	// Size of above arrays
	TFORM ShapeDispTForm;
	TFORM ShapeFileTForm;
	BOOL bRot;				// was last action a rotation (with siblings)
	} SHAPEEDIT, FAR* LPSHAPEEDIT;

