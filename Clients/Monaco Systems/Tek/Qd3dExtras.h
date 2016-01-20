#pragma once

#include "Qd3dAll.h"

TQ3GroupObject OpenModel( char* szFileName );
void SaveModel( char* szFileName, TQ3GroupObject aModel );
TQ3DrawContextObject MyNewDrawContext(HWND theWindow, LPRECT winrect);
TQ3CameraObject MyNewCamera(LPRECT winRect, TQ3Point3D *cameraFrom, TQ3Point3D *cameraTo);
TQ3GroupObject MyNewLights(double ambient, double point, double fill);
TQ3ViewObject MyNewView(HWND dp, LPRECT winrect, double ambient, double point, double fill,TQ3Point3D *from, TQ3Point3D *to);
TQ3GroupObject MyNewModel(void);
void SetQd3dErrorHandler(void);

