//////////////////////////////////////////////////////////////////////////////
//  tweaktab.h																//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#ifndef IN_TWEAKTAB
#define IN_TWEAKTAB

#include "mcostat.h"
#include "profiledocinf.h"
#include "windowtab.h"
#include "tweak_patch.h"
#include "mcoiccmac.h"

#include "QD3D.h"
#include "QD3DMath.h"
#include "QD3DDrawContext.h"
#include "QD3DShader.h"
#include "QD3DTransform.h"
#include "QD3DGroup.h"
#include "QD3DCamera.h"
#include "qd3Dwin.h"

class TweakTab:public WindowTab,QD3DWin {
private:
protected:
	RefConType	*refCon1;
	RefConType	*refCon2;
	ProfileDocInf	*doc;
	
	Tweak_Element	*current_tweak;
	int			tweak_num;
	
	double		SliderL;
	short		*_indexlut;
	double		_minL,_maxL;
	
	int			doneGamut;
	int			startGamut;
	
	double desiredRGB[3];
	
	double printedRGB[3];
	
	double gammutRGB[3];
	
	double	rgb_d[3],rgb_g[3],rgb_p[3];
	
	int 	validWin;
	
	
	long _last_tick_time2;
	long Current_Text;
	
	McoXformIccMac *lab_rgb_xform;
	
	McoHandle gammutSurfaceH;
	
	void _init();
	
	void _changeToCalib(void);
	void _changeToGamut(void);
	
	void _changeTweakData(int which);
	void _updatSliderL(void);
	
	void _setUpTweak(int which);
	McoStatus _setUpTweaks(void);
	void _setUpTweakMenu(void);
	McoStatus _copyTextFromDial(void);
	McoStatus _copyTextToDial(void);
	
	void _checkMaxVals(double *lab);
	void ConvertToRGB(double *lab,double *rgb);
	void ConvertToLAB(double *rgb,double *lab);
	void ConvertToGammutC(double *lab,double *labc);
	int isColorInGamut(double *lab);
	
	void _converttoTwPrint(double *lab,double *labo);
	void _converttoTwGamut(double *lab,double *labo);
	
	McoStatus NewTweak(void);
	McoStatus DeleteTweak(void);
	
	void Clear3DWin(void);
	
	// build a set of luts that is used to find the index into a gamutsurface
	McoStatus buildIndexLut(double *gammutSurface);
	long getgammutSurfaceIndex(double L,double h);
	
	McoStatus get3Dinfo(Point clickPoint,double *lab,int gamutCheck,int *ingamut);
	
	McoStatus SaveTweakedPatches(void);
public:
	TweakTab(ProfileDoc *dc,WindowPtr d);	
	~TweakTab(void);
	
	TQ3GeometryObject 	BlackBox(void);
	
	McoStatus AddVertex(int i,double *lab,TQ3Vertex3D *vertices,TQ3MeshVertex *meshVertices,TQ3GeometryObject geometryObject);
	McoStatus	ModifyGammutObject(McoHandle gammutSurfaceH);
	TQ3GeometryObject NewGammutObject(McoHandle gammutSurfaceH);
	TQ3GeometryObject New_ab_ax(void);
	short SetUpCircle(TQ3PolyLineData *linedata,double *lab1,double Lr,double Cr,double currentL);

	McoStatus ModifyCircle(double *lab,double Lr,double Cr,double currentL);
	TQ3GeometryObject NewCircle(double *lab1,double Lr,double Cr,double currentL);
	
	McoStatus ModifyArrow(double *lab1,double *lab2);
	// build an arrow
	TQ3GeometryObject NewArrow(double *lab1,double *lab2);
	
	TQ3GeometryObject NewPolyy(double *disp,double *polypoints,double scale,int num_pp,TQ3ColorRGB *color);
	
	McoStatus ModifyPoly(double *disp,double *polypoints,double scale,int num_pp);
	// make a cross 
	TQ3GeometryObject NewCross(double *lab);
	// modify a cross
	McoStatus ModifyCross(double *lab);

	
	
	// change the geometry of the object type with number num
	McoStatus ChangeGeometry (McoQ3DType type, long num);
	
	McoStatus updateWindowData(int changed);
	
	// Get the active region
	McoStatus makeEventRegion(RgnHandle previewRgn,RgnHandle mouseRgn);
	McoStatus doPointInWindowActiveRgn(Point where,WinEve_Cursors &cursortype);
	
	McoStatus CopyParameters(void);
	McoStatus UpdateWindow(void);
	Boolean isMyObject(Point where,short *item);
	Boolean TimerEventPresent(void);
	McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed);
	McoStatus SetUpDial(void);


	};

#endif