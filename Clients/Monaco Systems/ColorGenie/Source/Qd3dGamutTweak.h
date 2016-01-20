#pragma once

#include "Qd3dGamutBase.h"
#include "tweak_patch.h"

class CQd3dGamutTweak : public CQd3dGamutBase
{
private:
protected:
	Tweak_Element* _pTweak;
	double _SliderL;
	short* _indexlut;
	double _minL;
	double _maxL;

public:
	CQd3dGamutTweak();
	~CQd3dGamutTweak();
	int /*iFOVMultiplier*/ CreateGeometry(void);
	McoStatus ChangeGeometry(Qd3dType type, long num);
	void ChangeTweakData(Tweak_Element* pTweak, int iSliderL);
	TQ3GeometryObject NewBlackBox(void);
	McoStatus	ModifyGamutObject(void);
	TQ3GeometryObject NewGamutObject(BOOL bDidGamutCompression);
	McoStatus AddVertex(int i,double *lab,TQ3Vertex3D *vertices,TQ3MeshVertex *meshVertices,TQ3GeometryObject geometryObject);
	McoStatus buildIndexLut(double *gamutSurface);
	long getgamutSurfaceIndex(double L,double h);
	TQ3GeometryObject New_ab_ax(void);
	McoStatus ModifyCircle(void);
	TQ3GeometryObject NewCircle(void);
	short SetUpCircle(TQ3PolyLineData *linedata,double *lab1,double Lr,double Cr,double currentL);
	McoStatus ModifyArrow(void);
	TQ3GeometryObject NewArrow(void);
	TQ3GeometryObject NewPolyy(double *disp,double *polypoints,double scale,int num_pp,TQ3ColorRGB *color);
	McoStatus ModifyPoly(double *disp,double *polypoints,double scale,int num_pp);
	TQ3GeometryObject MakeCross(BOOL bNew);
};
