#pragma once

#include "Qd3dAll.h"
#include "GamutData.h"
#include "xyztorgb.h"

class CQd3dGamutBase
{
public:
	CGamutData _GamutData;
	double _ambient;
	double _point;
	double _fill;
	HWND _hWndGamut;
	McoHandle _hGamutData;
	BOOL	_deleteflag;
	XyztoRgb _XyztoRgb;
	double _monitor_z;
	DocumentRec Document;
	RECT _box_rect;

public:
	CQd3dGamutBase();
	~CQd3dGamutBase();
	McoStatus InitDocumentData(void);
	void DisposeDocumentData(void);
	McoStatus Init(char* szFileName, HWND hWnd);
//	McoStatus InitFromDoc(RawData* rawdata, HWND hWnd);
	McoStatus InitFromDoc(McoHandle gamut, HWND hWnd);

	virtual int /*iFOVMultiplier*/ CreateGeometry(void);
	McoHandle ReadGamutDataFile(char* szFileName);
	McoStatus ChangeGeometry(Qd3dType type, long num);
	TQ3Point3D AdjustCamera(short winWidth, short winHeight, int iFOVMultiplier);
	void GetGroupBBox( TQ3BoundingBox* viewBBox );
	TQ3Status DocumentDraw3DData(void);
	TQ3Status SubmitScene(void);
	void DoTransform(int code, double v1, double v2, double v3);
	void GetRotation(TQ3Matrix4x4* rotation);
	void SetRotation(TQ3Matrix4x4* rotation);
	void TranslateGlobalToLocal(TQ3Matrix4x4* LtoG, TQ3Point3D* GPoint,TQ3Point3D* LPoint);
	McoStatus Get3Dinfo(POINT clickPoint,double* lab,int gamutCheck,int* ingamut);
};
