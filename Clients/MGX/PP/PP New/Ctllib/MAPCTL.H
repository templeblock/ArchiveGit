// (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// mapctl.h for map control
#ifndef _MAPCTL_H_
#define _MAPCTL_H_

#ifdef CTLLIB
BOOL Map_RegisterClass (HINSTANCE hInstance, UINT ClassScope);
BOOL Map_UnRegisterClass (HINSTANCE hInstance);
#endif
long WINPROC Map_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
BOOL Map_Init(HWND hMap, int gridX, int gridY, double gamma, int MaxPoints, WORD wStyles);
void Map_SetCurveType(HWND hMap, int CurveType);
void Map_SetStyle(HWND hMap, WORD wStyle, BOOL bOn);
WORD Map_GetStyle(HWND hMap);
void Map_SetGamma(HWND hMap, double gamma);
int  Map_GetPointList(HWND hMap, LPPOINT lpResult, int MaxPoints, BOOL bBound = YES);
BOOL Map_SetPointList(HWND hMap, LPPOINT lpPoints, int nPoints);
void Map_ClearProbe(HWND hMap);
int  Map_SetProbe(HWND hMap, int x);
BOOL Map_AddPoint(HWND hMap, LPPOINT pt);
BOOL Map_GetActivePoint(HWND hMap, LPPOINT lpPoint);
BOOL Map_GetBrowsePoint(HWND hMap, LPPOINT lpPoint);


// styles
#define MS_CURVES		0x0001			// interpolate with curves??
#define MS_INVERTX		0x0002			// invert the x axis on display
#define MS_INVERTY		0x0004			// invert the y axis on display
#define MS_NOHANDLES	0x0008			// invert the y axis on display
#define MS_INVERT_GAMMA	0x0010			// should the gamma calculation be reversed?

// notifications 
#define MN_POINTCHANGED		1		// actual change in the map
#define MN_POINTCHANGING	2		// actual change in the map (while dragging)
#define MN_BROWSE			3		// change in brosing status only (no map change)

#endif // _MAPCTL_H_

