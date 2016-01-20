#pragma once

#include "Qd3dGamutBase.h"

class CQd3dGamutView : public CQd3dGamutBase
{
public:
	CQd3dGamutView();
	~CQd3dGamutView();
	int /*iFOVMultiplier*/ CreateGeometry(void);
	McoStatus ChangeGeometry(Qd3dType type, long num);
	TQ3GeometryObject NewGamutObject(void);
	TQ3GeometryObject NewPolyGCIELabRef(void);
	TQ3GeometryObject NewPolyLCIELabRef(void);
	TQ3GeometryObject NewLxyAx(void);
};
